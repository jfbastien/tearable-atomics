#!/bin/bash

set -e
set -u
set -x

SRC=./tearable-atomic.cc
CCFLAGS="-std=c++14 -stdlib=libc++ -S -O2"

clang++ $SRC $CCFLAGS -target x86_64-unknown-linux-gnu -mcx16 -o x86.S
clang++ $SRC $CCFLAGS -target armv7a-unknown-linux-gnueabihf -mfloat-abi=hard -o a32.S
clang++ $SRC $CCFLAGS -target armv7a-unknown-linux-gnueabihf -mfloat-abi=hard -mcpu=cortex-a15 -o a32-lpae.S
