#ifndef SRC_SL_ENTRY_H_
#define SRC_SL_ENTRY_H_

#include "include/sl_op.h"

class SlEntry {
public:
  SlEntry(SlOp* op)
    : match_(NULL),
    next_(NULL),
    prev_(NULL),
    op_(op),
    state_machine_(NULL) {
    }

  ~SlEntry() {
    delete SlOpSm;
  }

  bool IsCall() const {
    return match_ != NULL;
  }

  SlEntry* Next() const {
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
  
  void Link(SlEntry* entry) {
    next_ = entry;
    entry->prev_ = this;
  }

  bool Apply() {
    SlOp *res_op = op_->Apply(state_machine_);
    bool is_linearized = (*res_op == *(match_->op_));
    delete res_op;
    return is_linearized;
  }

private:
  SlEntry *match_;
  SlEntry *next_;
  SlEntry *prev_;

  SlOp* op_;
  SlOpSm* state_machine_;
  
  SlEntry* Prev() const {
    return prev_;
  }
};

#endif  // SRC_SL_ENTRY_H_
