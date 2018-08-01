#ifndef INCLUDE_SL_OP_H_
#define INCLUDE_SL_OP_H_
#include <string>

class SlOpSm {
public:
  virtual ~SlOpSm() {}
  virtual SlOpSm* Snap() const = 0;
  virtual void Recover(const SlOpSm* snap) = 0;
  virtual void Dump() const = 0;
  virtual bool Equal(const SlOpSm* sm) const = 0;
};

class SlOp {
public:
  SlOp()
    : id_(-1), is_call_(false), is_ok_(true) {}

  SlOp(int id, const std::string& who, bool call = false, bool ok = true)
    : id_(id), invoker_(who), is_call_(call), is_ok_(ok) {}

  virtual ~SlOp() {}

  virtual SlOp* Apply(SlOpSm *data) = 0;

  virtual void Dump() const {
    // Op[id, invoker, is_call]
    printf("Op[%d, %s, %s, %s], ", id_, invoker_.c_str(),
        is_call_ ? "invoke" : "return",
        is_ok_ ? "ok" : "failed");
  }
  
  virtual bool Equal(const SlOp* op) const = 0;

  int id() const {
    return id_;
  }

  bool is_call() const {
    return is_call_;
  }

  bool is_ok() const {
    return is_ok_;
  }
  
  std::string invoker() const {
    return invoker_;
  }

protected:
  int id_;
  std::string invoker_;
  bool is_call_;
  bool is_ok_;
};

#endif  // INCLUDE_SL_OP_H_
