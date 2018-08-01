#ifndef INCLUDE_SL_OP_KV_H_
#define INCLUDE_SL_OP_KV_H_

#include <string>
#include <unordered_map>
#include "include/sl_op.h"

enum SlKvOpType {
  kSet = 0,
  kGet,
  kDefault
};

class SlKvOpSm : public SlOpSm {
public:
  virtual ~SlKvOpSm() {}
  virtual SlOpSm* Snap() const {
    SlKvOpSm* snap = new SlKvOpSm();
    snap->kv_pair_ = kv_pair_;
    return snap;
  }

  virtual void Recover(const SlOpSm* snap) {
    const SlKvOpSm* s = dynamic_cast<const SlKvOpSm*>(snap);
    kv_pair_ = s->kv_pair_;
  }

  virtual void Dump() const {
    for(auto& item : kv_pair_) {
      printf("%s => %s\n", item.first.c_str(), item.second.c_str());
    }
  }

  virtual bool Equal(const SlOpSm *sm) const {
    const SlKvOpSm* s = dynamic_cast<const SlKvOpSm*>(sm);
    return kv_pair_ == s->kv_pair_;
  
  }

private:
  friend class SlKvOp;
  std::unordered_map<std::string, std::string> kv_pair_;
};

class SlKvOp : public SlOp {
public:
  SlKvOp()
    : SlOp(), op_type_(kDefault) {}

  SlKvOp(int id, const std::string& invoker, bool is_call,
      SlKvOpType t,
      const std::string k,
      const std::string v = std::string())
    : SlOp(id, invoker, is_call),
    op_type_(t),
    key_(k),
    value_(v) {}

  bool Init(const std::string& hline);

  virtual ~SlKvOp() {}

  virtual SlOp* Apply(SlOpSm *sm);

  virtual void Dump() const;

  virtual bool Equal(const SlOp* op) const;

private:
  SlKvOpType op_type_;
  std::string key_;
  std::string value_;
};

#endif  // INCLUDE_SL_OP_KV_H_
