#ifndef SRC_SL_BITSET_H_
#define SRC_SL_BITSET_H_
#include <map>
#include <sstream>
#include <cassert>

static inline uint64_t BitMask(uint64_t pos) {
  assert(pos < sizeof(uint64_t));
  return 1 << pos;
}

static inline uint64_t SliceNum(uint64_t pos) {
  return pos / sizeof(uint64_t);
}

static inline uint64_t SliceIndex(uint64_t pos) {
  return pos % sizeof(uint64_t);
}

class SlBitset {
public:

  void Alloc(uint64_t size) {
    uint64_t slice_cnt = size / sizeof(uint64_t) + 1;
    while (slice_cnt--) {
      slices_.insert(std::pair<uint64_t, uint64_t>(slice_cnt - 1, 0));
    }
  }

  void set(uint64_t pos) {
    assert(SliceNum(pos) < slices_.size());
    slices_[SliceNum(pos)] |= BitMask(SliceIndex(pos));
  }

  void unset(uint64_t pos) {
    assert(SliceNum(pos) < slices_.size());
    slices_[SliceNum(pos)] &= (~BitMask(SliceIndex(pos)));
  }

  std::string ToString() const {
    std::stringstream ss;
    for (auto& pair : slices_) {
      ss << pair.second;
    }
    return ss.str();
  }

private:
  std::map<uint64_t, uint64_t> slices_;
};

#endif  // SRC_SL_BITSET_H_
