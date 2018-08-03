#ifndef INCLUDE_SL_ENTRY_H_
#define INCLUDE_SL_ENTRY_H_
#include "include/sl_op.h" 
class SlEntry {
public:
  SlEntry(SlOp* op)
    : call_id_(-1),
    match_(NULL),
    next_(NULL),
    prev_(NULL),
    op_(op) {
    }

  ~SlEntry() {
  }

  int call_id() const {
    return call_id_;
  }

  void SetCallId(int id) {
    call_id_ = id;
  }

  bool IsCall() const {
    return match_ != NULL;
  }

  bool IsFake() const {
    return op_ == NULL;
  }

  SlEntry* Next() const {
    return next_;
  }

  void Lift() {
    prev_->next_ = next_;
    if (next_) {
      next_->prev_ = prev_;
    }
    
    if (match_) {
      match_->prev_->next_ = match_->next_;
      if (match_->next_) {
        match_->next_->prev_ = match_->prev_;
      }
    }
  }

  void Unlift() {
    match_->prev_->next_ = match_;
    if (match_->next_) {
      match_->next_->prev_ = match_;
    }
    prev_->next_ = this;
    next_->prev_ = this;
  }
  
  void Link(SlEntry* entry) {
    next_ = entry;
    entry->prev_ = this;
  }

  bool Apply(SlOpSm *sm) {
    SlOp *res_op = op_->Apply(sm);
    bool is_linearized = match_->IsFake()  // Unfinished call
      || !match_->op_->is_ok()   // Fail or Timeout
      || match_->op_->Equal(res_op);
    delete res_op;
    return is_linearized;
  }

  void SetMatch(SlEntry* entry) {
    match_ = entry;
  }

  void Dump() {
    printf("Dump entry %d Match: %d, Next: %d, Prev: %d\n",
        call_id_, match_ ? match_->call_id() : -1,
        next_ ? next_->call_id() : -1,
        prev_ ? prev_->call_id() : -1);
    if (op_) {
      op_->Dump();
    } else {
      printf("Fake Entry\n");
    }
  }

private:
  int call_id_;
  SlEntry *match_;
  SlEntry *next_;
  SlEntry *prev_;

  SlOp* op_;
  
  SlEntry* Prev() const {
    return prev_;
  }
};

#endif  // INCLUDE_SL_ENTRY_H_
