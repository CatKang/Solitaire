#ifndef SRC_SL_OP_H_
#define SRC_SL_OP_H_

class SlOpSm {
  // StateMachine Interface
  virtual ~SlOpSm() {}
};

class SlOp {
public:
  SlOp(int id, bool call = false)
    : id_(id), is_call_(call) {}

  virtual ~SlOp();

  virtual SlOp* Apply(SlOpSm *data) = 0;

private:
  int id_;
  bool is_call_;
};

#endif  // SRC_SL_OP_H_
