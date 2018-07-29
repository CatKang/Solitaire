# Solitaire (Developing)
Solitaire is a faster Linearizability checker supporting multi-interface.

Given a concurrent history, Solitaire will determine whether the history is Linearizability by finding a sequential specification. The basic algorithm is described in [Faster linearizability checking via P-compositionality](https://arxiv.org/pdf/1504.00204.pdf).
    
Solitaire is named after the famous Windows card game, which involve dealing cards from a shuffled deck into a prescribed arrangement on a tabletop.

## Usage
``` dash
# Compile
cd kv_checker
g++ -std=c++11 kv_checker.cc sl_op_kv.cc ../src/sl_checker.cc -I.. -o kv_checker

# Run
kv_checker test_demo.txt

```
