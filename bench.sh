#!/bin/bash

set -e
set -u
set -x

# benchmark from `git clone git@github.com:google/benchmark.git`
# Built with:
# (cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++11" && make -j50)
echo "benchmark is located at $BENCH"

BENCH_CC=./bench.cc
CCFLAGS="-std=c++11 -O2"

clang++ $BENCH_CC $CCFLAGS -target x86_64-unknown-linux-gnu -mcx16 \
  -lc++ \
  -I$BENCH/include/ -L$BENCH/src/ -lbenchmark \
  -o ./bench
./bench
