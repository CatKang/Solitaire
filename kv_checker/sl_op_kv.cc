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

bool SlKvOp::Init(const std::string& history_line) {
  std::vector<std::string> options, elems;
  std::string line = Trim(Trim(history_line, '{'), '}');
  Split(line, ',', &options);
 
  for (std::string opt: options) {
    Split(opt, ' ', &elems);
    int i = 0;
    if (elems[0].empty()) {
      i++;
    }

    key_ = "default";
    std::string key = elems[i];
    if (key == ":index") {
      id_ = stoi(elems[i+1]);
    } else if (key == ":process") {
      invoker_ = elems[i + 1];
    } else if (key == ":type") {
      // Invoke or return
      if (elems[i + 1] == ":invoke") {
        is_call_ = true;
      } else if (elems[i + 1] == ":ok") {
        is_call_ = false;
        ret_ = kSucc;
      } else if (elems[i + 1] == ":fail") {
        is_call_ = false;
        ret_ = kFail;
      } else {
        is_call_ = false;
        ret_ = kTimeout;
      }
    } else if (key == ":f") {
      std::string stype = elems[i + 1];
      if (stype == ":write") {
        op_type_ = kSet;
      } else if (stype == ":read") {
        op_type_ = kGet;
      } else {
        printf("unknown op type: %s\n", stype.c_str());
        return false;
      }
    } else if (key == ":key") {
      key_ = Trim(elems[i + 1], '"');
    } else if (key == ":value") {
      value_ = Trim(elems[i + 1], '"');
      if (value_ == "nil") {
        value_ = "";
      }
    } else if (key == ":time" || key == ":error") {
      // Skip
    } else {
      printf("unknown option: %s\n", key.c_str());
      exit(-1);
    }
  }

 // Dump();
  return true;
}

SlOp* SlKvOp::Apply(SlOpSm *sm) {
  if (!is_call_
      || !sm) {
    return new SlKvOp();
  }

  SlKvOpSm *kv_sm = dynamic_cast<SlKvOpSm*>(sm);
  if (kSet == op_type_) {
    kv_sm->kv_pair_[key_] = value_;
    return new SlKvOp(-1, invoker_, false, op_type_, key_, value_);
  } else {
    if (kv_sm->kv_pair_.find(key_) == kv_sm->kv_pair_.end()) {
      return new SlKvOp(-1, invoker_, false, op_type_, key_, "0"); // Treat 0 as not found
    }
    return new SlKvOp(-1, invoker_, false, op_type_, key_, kv_sm->kv_pair_[key_]);
  }
}

void SlKvOp::Dump() const {
  SlOp::Dump();
  printf("type: %s, key: %s, value: %s\n", (op_type_ == kSet ? "Set" : "Get"),
      key_.c_str(), value_.c_str());
}

bool SlKvOp::Equal(const SlOp* op) const {
  const SlKvOp* kv_op = dynamic_cast<const SlKvOp*>(op);
  return op_type_ == kv_op->op_type_
    && key_ == kv_op->key_
    && value_ == kv_op->value_;
}

