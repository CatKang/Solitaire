#ifndef SRC_SL_BITSET_H_
#define SRC_SL_BITSET_H_
#include <map>
#include <sstream>
#include <cassert>
#include <functional>

static const uint64_t kSliceLen = sizeof(uint64_t) * 8;

static inline uint64_t BitMask(uint64_t pos) {
  assert(pos < kSliceLen);
  return uint64_t(1) << pos;
}

static inline uint64_t SliceNum(uint64_t pos) {
  return pos / kSliceLen;
}

static inline uint64_t SliceIndex(uint64_t pos) {
  return pos % kSliceLen;
}

class SlBitset {
public:

  SlBitset()
    : base_slice_(0), base_hash_(17), hash_(base_hash_) {}

  void Alloc(uint64_t size) {
    uint64_t slice_cnt = size / kSliceLen + 1;
    while (slice_cnt--) {
      slices_.insert(std::pair<uint64_t, uint64_t>(slice_cnt, 0));
    }
  }

  void set(uint64_t pos) {
    uint64_t num = SliceNum(pos);
    assert(num < slices_.size());
    slices_[num] |= BitMask(SliceIndex(pos));
    //refresh_hash(slices_[num], num);
  }

  void unset(uint64_t pos) {
    uint64_t num = SliceNum(pos);
    assert(num < slices_.size());
    slices_[num] &= (~BitMask(SliceIndex(pos)));
    //refresh_hash(slices_[num], num);
  }

  bool operator==(const SlBitset& other) const {
    return slices_ == other.slices_;
  }

  bool operator!=(const SlBitset& other) const {
    return slices_ != other.slices_;
  }

private:
  friend struct SlBitsetHasher;
  std::map<uint64_t, uint64_t> slices_;
  uint64_t base_slice_;
  uint64_t base_hash_;
  std::size_t hash_;

  void refresh_hash(uint64_t new_s, uint64_t num) {
    if (num > base_slice_) {
      for (int i = base_slice_; i < num; ++i) {
        base_hash_ = base_hash_ * 31 + slices_[i];
      }
      base_slice_ = num;
    } else if (num < base_slice_) {
      for (int i = 0; i < num; ++i) {
        base_hash_ = 17 * 31 + slices_[i];
      }
      base_slice_ = num;
    }
    hash_ = base_hash_ * 31 + new_s;
  }

};

struct SlBitsetHasher {
  std::size_t operator()(const SlBitset& sb) const {
    // Compute hash follow http://stackoverflow.com/a/1646913/126995
    uint64_t hash = 17;
    for (auto& i : sb.slices_) {
      hash  = hash * 31 + std::hash<unsigned long long>()(i.second);
    }
    return hash;
  }
};

#endif  // SRC_SL_BITSET_H_
