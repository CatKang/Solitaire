#ifndef SRC_SL_OP_KV_H_
#define SRC_SL_OP_KV_H_

#include <string>

enum SlKvOpType {
  kSet = 0,
  kGet,
};

class SlKvOpSm : public SlOpSm {
public:
  std::string value;
};

class SlKvOp : public SlOp {
public:
  SlKvOp()
    : SlOp(-1, false), op_type_(0) {}

  SlKvOp(int id, bool is_call,
      SlKvOpType t, const std::string v = std::string())
    : SlOp(id, is_call),
    op_type_(t),
    value_(v) {}

  virtual ~SlKvOp() {}

  virtual SlOp* Apply(SlOpSm *sm) {
    if (!is_call_) {
      return SlKvOp();
    }

    if (!sm) {
      sm = new SlKvOpSm();
    }

    SlKvOpSm *kv_sm = dynamic_cast<SlKvOpSm*>(sm);
    if (kSet == op_type_) {
      kv_sm->value = value_;
      return new SlKvOp(id_, false, op_type_);
    } else {
      return new SlKvOp(id_, false, op_type_, kv_sm->value);
    }
  }
private:
  SlKvOpType op_type_;
  std::string value_;
};

#endif  // SRC_SL_OP_KV_H_
