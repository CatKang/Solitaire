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
    snap->kv_pair = kv_pair;
    return snap;
  }

  virtual void Recover(const SlOpSm* snap) {
    const SlKvOpSm* s = dynamic_cast<const SlKvOpSm*>(snap);
    kv_pair = s->kv_pair;
  }

  virtual void Dump() {
    for(auto& item : kv_pair) {
      printf("%s => %s\n", item.first.c_str(), item.second.c_str());
    }
  }

  std::unordered_map<std::string, std::string> kv_pair;
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

  explicit SlKvOp(int id, const std::string& hline);

  virtual ~SlKvOp() {}

  virtual SlOp* Apply(SlOpSm *sm);

  virtual void Dump() const;

  virtual bool Equal(SlOp* op) const;

private:
  SlKvOpType op_type_;
  std::string key_;
  std::string value_;
};

#endif  // INCLUDE_SL_OP_KV_H_
