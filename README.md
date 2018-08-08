# Solitaire
Solitaire is a faster Linearizability checker supporting multi-interface.

Given a concurrent history, Solitaire will determine whether the history is Linearizability by finding a sequential specification. The basic algorithm is described in [Faster linearizability checking via P-compositionality](https://arxiv.org/pdf/1504.00204.pdf).
    
Solitaire is named after the famous Windows card game, which involve dealing cards from a shuffled deck into a prescribed arrangement on a tabletop.

## Usage
``` dash
# Compile
cd kv_checker
g++ -std=c++11 -O2 kv_checker.cc sl_op_kv.cc ../src/sl_checker.cc ../src/sl_cache.cc -I.. -o kv_checker

# Run
kv_checker test/test_demo.txt

```

## Efficiency
![Compare with Jepsen](http://catkang.github.io/assets/img/test_linearizability/Linearizability_8.png)

## Comming Soon
- Optimize bitset hash
- Support Multi-thread
