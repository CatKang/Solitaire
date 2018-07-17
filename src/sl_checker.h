#ifndef SRC_SL_CHECKER_H_
#define SRC_SL_CHECKER_H_

#include <stack>
#include <vector>
#include "include/sl_op.h"
#include "include/sl_entry.h"

class SlChecker {
public:
  SlChecker();
  ~SlChecker();

  int Init(std::vector<SlOp> ops);
  bool Check();

private:
  Entry *head_;
  int entry_cnt_;
  std::stack<Entry*> calls_;

};

#endif  // SRC_SL_CHECKER_H_
