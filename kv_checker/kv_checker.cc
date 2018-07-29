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
  std::vector<SlOp*> ops;
  int id = 1;
  while (getline(fin, line)) {
    if (line.empty()) {
      continue;
    }
    SlKvOp *op = new SlKvOp(id++, line);
    ops.push_back(op);
  }

  SlKvOpSm sm;
  SlChecker checker;
  if (!checker.Init(ops, &sm)) {
    printf("checker init failed!\n");
  }

  printf("Linearizability: %s\n", checker.Check() ? "true" : "false");
  
  for (SlOp* op : ops) {
    delete op;
  }

  return 0;
}
