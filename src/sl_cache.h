#ifndef SRC_SL_CACHE_H_
#define SRC_SL_CACHE_H_
#include <deque>
#include <unordered_map>
#include "include/sl_op.h"
#include "src/sl_bitset.h"

// TODO LRU
class SlCache {
public:
  ~SlCache();

  bool TryInsert(const SlBitset& bitset, const SlOpSm* sm);

private:
  std::unordered_map<std::string, std::deque<const SlOpSm*>> histories_;
};

#endif  // SRC_SL_CACHE_H_
