#include <unordered_map>
#include "include/sl_entry.h"
#include "include/sl_checker.h"

SlChecker::SlChecker()
  : head_(NULL) {
  }

SlChecker::~SlChecker() {
  SlEntry *cur = head_.Next(), *tmp = NULL;
  while (cur) {
    tmp = cur;
    cur = cur->Next();
    delete tmp;
  }
}

bool SlChecker::Init(const std::map<int, SlOp*> ops, SlOpSm* sm) {
  SlEntry *tail = &head_;
  std::unordered_map<std::string, SlEntry*> no_rets;

  printf("\nChecker Init------------------------\n");
  // Init Entry list
  for (auto& op_pair : ops) {
    SlOp* op = op_pair.second;

    printf("Link op: ");
    SlEntry *entry = new SlEntry(op);
    tail->Link(entry);
    printf("Link Entry: %d\n", entry->Id());
    entry->Dump();
    tail = tail->Next();
    
    // Set match entry
    if (op->is_call()) {
      printf("op is call, ");
      if (no_rets.find(op->invoker()) != no_rets.end()) {
        printf("call already in waiting ------------------------\n");
        return false;
      }
      no_rets[op->invoker()] = entry;
    } else {
      printf("op is not call,");
      if (no_rets.find(op->invoker()) == no_rets.end()) {
        printf("call in waiting not found ----------------------------\n");
        return false;
      }
      no_rets[op->invoker()]->SetMatch(entry);
      printf("set match for %d\n", no_rets[op->invoker()]->Id());
      no_rets.erase(op->invoker());
    }
    printf("\n");
  }

  // Add return entry for timeout op
  for (auto& wait : no_rets) {
    printf("Link Fake\n");
    SlEntry *ret_entry = new SlEntry(NULL);
    tail->Link(ret_entry);
    tail = tail->Next();
  }

  // Init StateMachine
  sm_ = sm;
  printf("-----------------------------\n");
  return true;
}

bool SlChecker::Check() {
  printf("\nChecker Check------------------------\n");
  SlEntry *entry = head_.Next();
  while (entry) {
    entry->Dump();
    if (entry->IsCall()) {
      // A call entry 
      printf("A call entry, ");
      SlOpSm *snap_sm = sm_->Snap();
      bool is_linearized = entry->Apply(sm_);
      if (is_linearized) {
        printf("is linearized, Lift");
        entry->Lift();
        calls_.push(std::pair<SlEntry*, SlOpSm*>(entry, snap_sm));
        printf(", Stack push: %d, ", entry->Id());
        snap_sm->Dump();

        entry  = head_.Next();
      } else {
        delete snap_sm;
        printf("is not linearized, Next\n");
        entry = entry->Next();
      }

    } else {
      // A return entry
      printf("A return entry, ");
      if (calls_.empty()) {
        printf("no more calls when next is return, ");
        printf("-----------------------------\n");
        return false;
      }
      std::pair<SlEntry*, SlOpSm*> b_pair= calls_.top();
      calls_.pop();
      b_pair.first->Unlift();
      entry = b_pair.first->Next();
      sm_->Recover(b_pair.second);
      printf(", Stack pop: %d, ", b_pair.first->Id());
      b_pair.second->Dump();
      delete b_pair.second;
      printf("Unlift one\n");
    }
    printf("\n");
  }
  printf("-----------------------------\n");

  return true;
}


