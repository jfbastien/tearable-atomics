#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <random>

#include "benchmark/benchmark_api.h"

#define NOINLINE __attribute__((noinline))

struct alignas(sizeof(intptr_t) * 2) Node {
  intptr_t l, r;
};

// Don't do anything with the Node: cmpxchg will always succeed.
struct Noop {
  NOINLINE Node operator()(Node n) { return n; }
};

template <typename Action>
struct Relaxed {
  void operator()(std::atomic<Node> *n) {
    Action act;
    // Relaxed loads can't tear.
    Node old = n->load(std::memory_order_relaxed);
    while (!n->compare_exchange_weak(old, act(old), std::memory_order_release,
                                     std::memory_order_relaxed))
      ;
  }
};

template <typename Action>
struct Tearing {
  void operator()(std::atomic<Node> *n) {
    Action act;
    Node *alias = reinterpret_cast<Node *>(n);
    Node old;
    // Tearable load.
    asm("movq %[ptr], %[l]" : [l] "=r"(old.l) : [ptr] "o"(alias->l));
    asm("movq %[ptr], %[r]" : [r] "=r"(old.r) : [ptr] "o"(alias->r));
    while (!n->compare_exchange_weak(old, act(old), std::memory_order_release,
                                     std::memory_order_relaxed))
      ;
  }
};

template <typename Op>
static void B(benchmark::State &state) {
  Op op;
  const auto num_atomics = state.range_x();
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<> disribution(0, num_atomics);
  static std::atomic<Node> *nodes;
  if (state.thread_index == 0) {  // Test initialization for all threads.
    // Force a large allocation to ensure alignment.
    size_t page_aligned_size = 1 << 16;
    size_t min_size = sizeof(std::atomic<Node>) * num_atomics;
    nodes = static_cast<std::atomic<Node> *>(
        malloc(std::max(page_aligned_size, min_size)));
  }

  while (state.KeepRunning()) {
    // Take one of the atomics randomly, and cmpxchg it.
    // Some of these atomics will exhibit false-sharing on purpose.
    std::atomic<Node> *node = &nodes[disribution(generator)];
    op(node);
  }

  if (state.thread_index == 0) {  // Test teardown for all threads.
    free(nodes);
  }
}

constexpr int max_threads = 256;
constexpr int max_atomics = 1 << 9;
BENCHMARK_TEMPLATE(B, Relaxed<Noop>)
    ->ThreadRange(1, max_threads)
    ->Range(1, max_atomics)
    ->UseRealTime()
    ->MinTime(1.0)
    ->Repetitions(10);
BENCHMARK_TEMPLATE(B, Tearing<Noop>)
    ->ThreadRange(1, max_threads)
    ->Range(1, max_atomics)
    ->UseRealTime()
    ->MinTime(1.0)
    ->Repetitions(10);

BENCHMARK_MAIN();
