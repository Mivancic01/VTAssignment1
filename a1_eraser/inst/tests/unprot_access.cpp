#include <inttypes.h>
#include <iostream>
#include <thread>

struct S {
public:
  uint64_t secret;
  S() : secret(4242){};
  void plus();
  void minus();
};

void S::plus() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1)); //hack
  secret += 42;
}

void S::minus() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  secret -= 42;
}

int main() {
  S s;
  std::thread t1(&S::plus, std::ref(s));
  std::thread t2(&S::minus, std::ref(s));
  t1.join();
  t2.join();
  std::cout << "s.secret is " << s.secret << "\n";
}