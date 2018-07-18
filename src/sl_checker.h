#ifndef SRC_SL_CHECKER_H_
#define SRC_SL_CHECKER_H_

#include <stack>
#include <vector>

class Entry;

class SlChecker {
public:
  SlChecker();
  ~SlChecker();

  int Init(std::vector<SlOp*> ops);
  bool Check();

private:
  Entry head_;
  std::stack<Entry*> calls_;

};

#endif  // SRC_SL_CHECKER_H_
