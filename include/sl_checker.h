#ifndef INCLUDE_SL_CHECKER_H_
#define INCLUDE_SL_CHECKER_H_

#include <stack>
#include <vector>
#include <utility>
#include "include/sl_entry.h"

class SlChecker {
public:
  SlChecker();
  ~SlChecker();

  bool Init(std::vector<SlOp*> ops, SlOpSm* sm);
  bool Check();

private:
  SlEntry head_;
  std::stack<std::pair<SlEntry*, SlOpSm*>> calls_;
  SlOpSm *sm_;
};

#endif  // INCLUDE_SL_CHECKER_H_
