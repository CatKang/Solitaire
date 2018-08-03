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
  std::string s_bitset = bitset.ToString();
  if (histories_.find(s_bitset) == histories_.end()) {
    std::deque<const SlOpSm*> sms;
    sms.push_back(sm->Snap());
    histories_.insert(std::pair<std::string, std::deque<const SlOpSm*>>(s_bitset, sms));
    return true;
  }

  for (auto& ptr : histories_[s_bitset]) {
    if (ptr->Equal(sm)) {
      return false;
    }
  }
  histories_[s_bitset].push_back(sm->Snap());
  return true;
}

