#include "src/sl_entry.h"
#include "src/sl_checker.h"

SlChecker::SlChecker()
  : head_(NULL) {
  }

SlChecker::~SlChecker() {
  Entry *cur = head_.Next(), *tmp = NULL;
  while (cur) {
    tmp = cur;
    cur = cur->Next();
    delete tmp;
  }
}

int SlChecker::Init(std::vector<SlOp*> ops) {
  SlEntry *tail = &head_;
  for (SlOp *op : ops) {
    SlEntry *entry = new SlEntry(op);
    tail->Link(entry);
    // TODO set match
  }
}

bool SlChecker::Check() {
  SlEntry *entry = head_.Next();
  while (entry) {
    if (entry->IsCall()) {
      // A call entry 
      bool is_linearized = entry->Apply();
      if (is_linearized) {
        entry->Lift();
        calls_.push(entry);
        entry  = head_.Next();
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


