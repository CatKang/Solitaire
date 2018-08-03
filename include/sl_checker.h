#ifndef INCLUDE_SL_CHECKER_H_
#define INCLUDE_SL_CHECKER_H_

#include <map>
#include <deque>
#include <vector>
#include <utility>
#include "include/sl_entry.h"

class SlBitset;
class SlCache;
class SlChecker {
public:
  SlChecker();
  ~SlChecker();

  bool Init(std::map<int, SlOp*> ops, SlOpSm* sm);
  bool Check();

  void DumpResult() const;

private:
  SlEntry head_;
  std::deque<std::pair<SlEntry*, SlOpSm*>> calls_;
  std::vector<int> longest_;
  SlOpSm *sm_;
  SlCache* cache_;
  SlBitset* bitset_;
};

#endif  // INCLUDE_SL_CHECKER_H_
