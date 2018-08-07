 #include <cassert>
#include <utility>
#include "src/sl_cache.h"

SlCache::~SlCache() {
  for (auto& pair : histories_) {
    for (auto& ptr : pair.second) {
      delete ptr;
    }
  }
}

bool SlCache::TryInsert(const SlBitset& bitset, const SlOpSm* sm) {
  if (histories_.find(bitset) == histories_.end()) {
    std::deque<const SlOpSm*> sms;
    sms.push_back(sm->Snap());
    histories_.insert(std::pair<SlBitset, std::deque<const SlOpSm*>>(bitset, sms));
    return true;
  }

  for (auto& ptr : histories_[bitset]) {
    if (ptr->Equal(sm)) {
      return false;
    }
  }
  histories_[bitset].push_back(sm->Snap());
  return true;
}

