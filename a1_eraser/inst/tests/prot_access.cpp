#include <inttypes.h>
#include <iostream>
#include <mutex>
#include <thread>

struct S {
public:
  uint64_t secret;
  std::mutex mux;
  S() : secret(4242){};
  void plus();
  void minus();
};

void S::plus() {
  mux.lock();
  secret += 42;
  mux.unlock();
}

void S::minus() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  mux.lock();
  secret -= 42;
  mux.unlock();
}

int main() {
  S s;
  std::thread t1(&S::plus, std::ref(s));
  std::thread t2(&S::minus, std::ref(s));
  t1.join();
  t2.join();
  s.mux.lock();
  std::cout << "s.secret is " << s.secret << "\n";
  s.mux.unlock();
  return 0;
}