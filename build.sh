#!/bin/bash

set -e
set -u
set -x

TEARABLE_CC=./tearable-atomic.cc
CCFLAGS="-std=c++14 -stdlib=libc++ -O2"

clang++ $TEARABLE_CC $CCFLAGS -S -target x86_64-unknown-linux-gnu -mcx16 \
  -o x86.S
clang++ $TEARABLE_CC $CCFLAGS -S -target armv7a-unknown-linux-gnueabihf \
  -mfloat-abi=hard \
  -o a32.S
clang++ $TEARABLE_CC $CCFLAGS -S -target armv7a-unknown-linux-gnueabihf \
  -mfloat-abi=hard -mcpu=cortex-a15 \
  -o a32-lpae.S
clang++ $TEARABLE_CC $CCFLAGS -S -target armv8-unknown-linux \
  -o a64.S
