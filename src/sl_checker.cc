#include <vector>
#include <unordered_map>
#include "include/sl_entry.h"
#include "include/sl_checker.h"
#include "src/sl_bitset.h"
#include "src/sl_cache.h"

SlChecker::SlChecker()
  : head_(NULL) {
    bitset_ = new SlBitset(); cache_ = new SlCache();
  }

SlChecker::~SlChecker() {
  SlEntry *cur = head_.Next(), *tmp = NULL;
  while (cur) {
    tmp = cur;
    cur = cur->Next();
    delete tmp;
  }
  for (auto& c : calls_) {
    delete c.first;
    delete c.second;
  }
  delete cache_;
  delete bitset_;
}

bool SlChecker::Init(const std::map<int, SlOp*> ops, SlOpSm* sm) {
  SlEntry *tail = &head_;
  std::unordered_map<std::string, SlEntry*> no_rets;
  std::vector<SlEntry*> fails;

  printf("\nChecker Init------------------------\n");
  // Init Entry list
  int call_id = 1;
  for (auto& op_pair : ops) {
    SlOp* op = op_pair.second;

    SlEntry *entry = new SlEntry(op);
    
    // Set match entry
    if (op->is_call()) {
      if (no_rets.find(op->invoker()) != no_rets.end()) {
        printf("call already in waiting ------------------------\n");
        return false;
      }
      entry->SetCallId(call_id++);
      no_rets[op->invoker()] = entry;
      tail->Link(entry);
      tail = tail->Next();
    } else {
      if (no_rets.find(op->invoker()) == no_rets.end()) {
        printf("call in waiting not found ----------------------------\n");
        return false;
      }
      SlEntry *invoke_entry = no_rets[op->invoker()];
      if (op->is_timeout()) {
        fails.push_back(invoke_entry);
      } else if (op->is_ok()) {
        tail->Link(entry);
        tail = tail->Next();
        invoke_entry->SetMatch(entry);
        entry->SetCallId(invoke_entry->call_id());
      } else {
        invoke_entry->Lift();
        delete entry;
        delete invoke_entry;
      }
      no_rets.erase(op->invoker());

    }
#ifdef DEBUG
    if (!op->is_fail()) {
      entry->Dump();
      printf("\n");
    }
#endif
  }

  // Add return entry for no return or timeout op
  for (auto& wait : no_rets) {
    fails.push_back(wait.second);
  }
  for (auto& fail : fails) {
    SlEntry *ret_entry = new SlEntry(NULL);
    tail->Link(ret_entry);
    tail = tail->Next();
    
    fail->SetMatch(ret_entry);
    ret_entry->SetCallId(fail->call_id());
#ifdef DEBUG
    ret_entry->Dump();
    printf("\n");
#endif
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
  longest_.clear();
  while (entry) {
#ifdef DEBUG
    entry->Dump();
#endif
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
        calls_.push_back(std::pair<SlEntry*, SlOpSm*>(entry, snap_sm));
        if (calls_.size() > longest_.size()) {
          longest_.clear();
          for(const auto& c : calls_) {
            longest_.push_back(c.first->call_id());
          }   
        }
#ifdef DEBUG
        printf("Lift: %d\n", entry->call_id());
        snap_sm->Dump();
#endif

        entry  = head_.Next();
      } else {
        // Recover and try next
        bitset_->unset(entry->call_id());
        sm_->Recover(snap_sm);
        delete snap_sm;
#ifdef DEBUG
        printf("Skip: %d, since: %s\n", entry->call_id(),
            is_linearized ? "Cache hit" : "Not Linearized");
#endif
        entry = entry->Next();
      }

    } else {
      // A return entry
      if (calls_.empty()) {
        printf("no more calls when next is return, ");
        printf("-----------------------------\n");
        return false;
      }
      std::pair<SlEntry*, SlOpSm*> b_pair= calls_.back();
      calls_.pop_back();
      b_pair.first->Unlift();
      bitset_->unset(b_pair.first->call_id());
      entry = b_pair.first->Next();
      sm_->Recover(b_pair.second);
#ifdef DEBUG
      printf("Unlift: %d\n", b_pair.first->call_id());
      b_pair.second->Dump();
#endif
      delete b_pair.second;
    }
#ifdef DEBUG
    printf("\n");
#endif
  }
  printf("-----------------------------\n");

  return true;
}

void SlChecker::DumpResult() const {
  for (int i : longest_) {
    printf("%d, ", i);
  }
  printf("\n");
}


