#ifndef INCLUDE_SL_ENTRY_H_
#define INCLUDE_SL_ENTRY_H_

#include "include/sl_op.h"

enum SlEntryType {
  kCall = 0,
  kRet,
};

class SlEntry {
public:
  virtual ~SlEntry();

  int Init(std::vector<SlOp> ops);

  bool IsCall() {
    return type_ == kCall;
  }

  SlEntry* Next() {
    return next_;
  }

  void Lift() {
    prev_->Next() = next_;
    next_->Prev() = prev_;
    match_->Prev()->Next() = match_->Next();
    if (match_->Next()) {
      match_->Next()->Prev() = match_->Prev();
    }
  }

  void Unlift() {
    match_->Prev()->Next() = match_;
    if (match_->Next()) {
      match_->Next()->Prev() = match_;
    }
    prev_->Next() = this;
    next_->Prev() = this;
  }

  virtual bool Apply() = 0;

private:
  SlEntryType type_;
  SlEntry *match_;
  SlEntry *next_;
  SlEntry *prev_;
  
  SlEntry* Prev() {
    return prev_;
  }
};

#endif  // INCLUDE_SL_ENTRY_H_
