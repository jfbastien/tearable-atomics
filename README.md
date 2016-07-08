# Tearable atomics

Is it useful for C++ to support "tearable" atomic memory ordering, where the
access participates in atomic ordering as strongly as `memory_order_relaxed`
accesses, but where the memory is allowed to tear (i.e. isn't single-copy
atomic)? In C++ standards speak: particular atomic object are **not**
indivisible with respect to all other atomic accesses to that object.

Naïvely this seems useless. It could be interesting when all lock-free
operations (including load/store) *must* be implemented as a
compare-and-exchange or load-linked/store-conditional:

* On recent x86 using `cmpxchg16b`.
* On A32 without LPAE using `ldrexd`, `clrex`, and `strexd`.

This memory access can then be used as a tearable load/store, potentially
cheaper than a compare-and-exchange, as long as a compare-and-exchange retry
loop follows it to handle races. If tearing occurs then the compare-and-exchange
does the right thing.

And, of course, `memory_order_tearing` has the neat properly of being spelled
with the same number of characters as the other 6 memory orderings.

# Is it faster?

`bench.cc` contains a benchmark for the tearing and relaxed approaches.
