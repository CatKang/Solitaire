#include <cstdio>
#include <fstream>
#include <string>
#include "sl_op_kv.h"
#include "include/sl_checker.h"

void usage() {
  printf("Usage: kv_checker path\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
    return -1;
  }

  std::ifstream fin(argv[1]);
  if (!fin.is_open()) {
    printf("History file %s can't open, may not exist!\n", argv[1]);
    usage();
    return -1;
  }

  std::string line;
  std::map<int, SlOp*> ops;
  int id = 1;
  while (getline(fin, line)) {
    if (line.empty()) {
      continue;
    }
    SlKvOp *op = new SlKvOp(id++, line);
    ops.insert(std::pair<int, SlOp*>(op->id(), op));
  }

  SlKvOpSm sm;
  SlChecker checker;
  if (!checker.Init(ops, &sm)) {
    printf("checker init failed!\n");
  }

  printf("Linearizability: %s\n", checker.Check() ? "true" : "false");
  
  for (auto& op : ops) {
    delete op.second;
  }

  return 0;
}
