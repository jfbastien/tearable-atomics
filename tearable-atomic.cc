#include <atomic>

struct alignas(sizeof(void *) * 2) Node {
  Node *l, *r;
};

Node single(Node *n) { return *n; }

Node multi(std::atomic<Node> *n) { return n->load(std::memory_order_relaxed); }

Node frob(Node n);

void mod(std::atomic<Node> *n) {
  Node old = n->load(std::memory_order_relaxed);
  while (!n->compare_exchange_weak(old, frob(old), std::memory_order_release,
                                   std::memory_order_relaxed))
    ;
}
