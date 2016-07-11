# Tearable atomics

Is it useful for C++ to support "tearable" atomic memory ordering, where the
access participates in atomic ordering as strongly as `memory_order_relaxed`
accesses, but where the memory is allowed to tear (i.e. isn't single-copy
atomic)? In C++ standards speak: particular atomic object are **not**
indivisible with respect to all other atomic accesses to that object.

Naïvely this seems useless. It nonetheless could be useful when all lock-free
operations (including load/store) *must* be implemented as a
compare-and-exchange or load-linked/store-conditional:

* On recent x86 using `cmpxchg16b`.
* On A32 without LPAE using `ldrexd`, `clrex`, and `strexd`.

This memory access can then be used as a tearable load/store, potentially
cheaper than a compare-and-exchange, as long as a compare-and-exchange retry
loop follows it to handle races. If tearing occurs then the compare-and-exchange
does the right thing.

* On x86 using two `movq` instructions (two instructions are never locked and
  can tear).
* On A32 using `ldrd` (without LPAE the instruction isn't single-copy atomic).

And, of course, `memory_order_tearing` has the neat properly of being spelled
with the same number of characters as the other 6 memory orderings.

# Is it faster?

`bench_tearing.cc` contains a benchmark for the tearing and relaxed approaches.

Try it out:

```
(git clone https://github.com/jfbastien/tearable-atomics.git && \
    cd tearable-atomics && \
    git submodule init && \
    git submodule update && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make -j`nproc` && \
    ./bench_tearing)
```

The benchmark will automatically scale to a multiple of the number of cores you
have available.

You can run each test multiple times, obtaining average and standard deviation,
and output to CSV by using:

```
./bench_tearing --benchmark_repetitions=10 --benchmark_format=csv
```
