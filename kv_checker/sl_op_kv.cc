#include <sstream>
#include <vector>
#include "sl_op_kv.h"

static void Split(const std::string& line, char delim,
    std::vector<std::string> *elems) {
  elems->clear();
  std::stringstream ss(line);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems->push_back(item);
  }
}

static std::string Trim(const std::string& str, char target) {
  if (str.empty()) {
    return str;
  }
  int begin = 0, end = str.size() - 1;
  while (str[begin] == target) {
    begin++;
  }
  while (str[end] == target) {
    end--;
  }
  return str.substr(begin, end - begin + 1);
}

SlKvOp::SlKvOp(int id, const std::string& history_line) {
  std::vector<std::string> options, elems;
  Split(history_line, ',', &options);
 
  id_ = id;
  for (std::string opt: options) {
    Split(opt, ' ', &elems);
    int i = 0;
    if (elems[0].empty()) {
      i++;
    }

    std::string key = elems[i];
    if (key == ":process") {
      invoker_ = elems[i + 1];
    } else if (key == ":type") {
      // Invoke or return
      is_call_ = true;
      if (elems[i + 1] == ":ok") {
        is_call_ = false;
      } 
    } else if (key == ":get") {
      op_type_ = kGet;
    } else if (key == ":set") {
      op_type_ = kSet;
    } else if (key == ":key") {
      key_ = Trim(elems[i + 1], '"');
    } else if (key == ":value") {
      value_ = Trim(elems[i + 1], '"');
    } else {
      printf("unknown option\n");
    }
  }

  Dump();
}

SlOp* SlKvOp::Apply(SlOpSm *sm) {
  if (!is_call_
      || !sm) {
    return new SlKvOp();
  }

  SlKvOpSm *kv_sm = dynamic_cast<SlKvOpSm*>(sm);
  if (kSet == op_type_) {
    kv_sm->kv_pair[key_] = value_;
    return new SlKvOp(-1, invoker_, false, op_type_, key_, value_);
  } else {
    if (kv_sm->kv_pair.find(key_) == kv_sm->kv_pair.end()) {
      return new SlKvOp();
    }
    return new SlKvOp(-1, invoker_, false, op_type_, key_, kv_sm->kv_pair[key_]);
  }
}

void SlKvOp::Dump() const {
  SlOp::Dump();
  printf("type: %s, key: %s, value: %s\n", (op_type_ == kSet ? "Set" : "Get"),
      key_.c_str(), value_.c_str());
}

bool SlKvOp::Equal(SlOp* op) const {
  SlKvOp* kv_op = dynamic_cast<SlKvOp*>(op);
  return op_type_ == kv_op->op_type_
    && key_ == kv_op->key_
    && value_ == kv_op->value_;
}

