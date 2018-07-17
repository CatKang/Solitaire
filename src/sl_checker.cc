#include "src/sl_checker.h"

SlChecker::SlChecker()
  : head_(NULL) {
  }

SlChecker::~SlChecker() {
  Entry *tmp;
  while (head_) {
    tmp = head_->Lift();
    delete tmp;
  }
}

int SlChecker::Init(std::vector<SlOp> ops) {
  for (SlOp op : ops) {
    head_->Link(op);
  }
}

bool SlChecker::Check() {
  SlEntry *entry = head_->Next();
  while (entry) {
    if (entry->IsCall()) {
      // A call entry 
      bool is_linearized = entry->Apply();
      if (is_linearized) {
        entry->Lift();
        calls_.push(entry);
        entry  = head_->Next();
      } else {
        entry = entry->Next();
      }
    } else {
      // A return entry
      if (calls_.empty()) {
        return false;
      }
      SlEntry *b_entry = calls_.top();
      calls_.pop();
      b_entry->Unlift();
      entry = b_entry->Next();
    }
  }
  return true;
}


