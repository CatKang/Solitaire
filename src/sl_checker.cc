#include <unordered_map>
#include "include/sl_entry.h"
#include "include/sl_checker.h"
#include "src/sl_bitset.h"
#include "src/sl_cache.h"

SlChecker::SlChecker()
  : head_(NULL) {
    bitset_ = new SlBitset();
    cache_ = new SlCache();
  }

SlChecker::~SlChecker() {
  SlEntry *cur = head_.Next(), *tmp = NULL;
  while (cur) {
    tmp = cur;
    cur = cur->Next();
    delete tmp;
  }
  delete cache_;
  delete bitset_;
}

bool SlChecker::Init(const std::map<int, SlOp*> ops, SlOpSm* sm) {
  SlEntry *tail = &head_;
  std::unordered_map<std::string, SlEntry*> no_rets;

  printf("\nChecker Init------------------------\n");
  // Init Entry list
  int call_id = 1;
  for (auto& op_pair : ops) {
    SlOp* op = op_pair.second;

    SlEntry *entry = new SlEntry(op);
    tail->Link(entry);
    tail = tail->Next();
    
    // Set match entry
    if (op->is_call()) {
      if (no_rets.find(op->invoker()) != no_rets.end()) {
        printf("call already in waiting ------------------------\n");
        return false;
      }
      entry->SetCallId(call_id++);
      no_rets[op->invoker()] = entry;
    } else {
      if (no_rets.find(op->invoker()) == no_rets.end()) {
        printf("call in waiting not found ----------------------------\n");
        return false;
      }
      SlEntry *invoke_entry = no_rets[op->invoker()];
      invoke_entry->SetMatch(entry);
      entry->SetCallId(invoke_entry->call_id());
      no_rets.erase(op->invoker());
    }
    entry->Dump();
    printf("\n");
  }

  // Add return entry for timeout op
  for (auto& wait : no_rets) {
    SlEntry *ret_entry = new SlEntry(NULL);
    tail->Link(ret_entry);
    tail = tail->Next();
    
    wait.second->SetMatch(ret_entry);
    ret_entry->SetCallId(wait.second->call_id());
    ret_entry->Dump();
    printf("\n");
  }

  // Init StateMachine
  sm_ = sm;

  // Alloc Bitset
  bitset_->Alloc(call_id);
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
      SlOpSm *snap_sm = sm_->Snap();
      bool is_linearized = entry->Apply(sm_), pick = false;
      if (is_linearized) {
        bitset_->set(entry->call_id());
        pick = cache_->TryInsert(*bitset_, sm_);
      }

      if (pick) {
        entry->Lift();
        calls_.push(std::pair<SlEntry*, SlOpSm*>(entry, snap_sm));
        printf("Lift: %d\n", entry->call_id());
        snap_sm->Dump();

        entry  = head_.Next();
      } else {
        // Recover and try next
        bitset_->unset(entry->call_id());
        sm_->Recover(snap_sm);
        delete snap_sm;
        printf("Skip: %d, since: %s\n", entry->call_id(),
            is_linearized ? "Cache hit" : "Not Linearized");
        entry = entry->Next();
      }

    } else {
      // A return entry
      if (calls_.empty()) {
        printf("no more calls when next is return, ");
        printf("-----------------------------\n");
        return false;
      }
      std::pair<SlEntry*, SlOpSm*> b_pair= calls_.top();
      calls_.pop();
      b_pair.first->Unlift();
      bitset_->unset(b_pair.first->call_id());
      entry = b_pair.first->Next();
      sm_->Recover(b_pair.second);
      printf("Unlift: %d\n", b_pair.first->call_id());
      b_pair.second->Dump();
      delete b_pair.second;
    }
    printf("\n");
  }
  printf("-----------------------------\n");

  return true;
}


