#include "../AdvancedEraser.h"
#include "../EraserLock.h"
#include "../EraserObject.h"
#include "../SimpleEraser.h"

#include <iostream>
#include <check.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

#include "util.h"

static std::string prefix;

START_TEST(sanity_check) {
  using namespace eraser;
  current_logger->clear();
  EraserObject<int> var1("var1");
  EraserLock lock1("lock1");
  lock1.lock();
  var1 = 1;
  lock1.unlock();
  int var2 = 0;
  var2 = 2;
  lock1.lock();
  var1 = var1() + var2;
  lock1.unlock();
  current_logger->show();
  ck_assert(util::check_correct());
}
END_TEST

START_TEST(uninit_access) {
  using namespace eraser;
  current_logger->clear();
  EraserObject<int> var1("var1");
  std::cout << "var1 value is " << var1() << std::endl;
  var1 = 10;
  std::cout << "var1 value is " << var1() << std::endl;
  EraserObject<int> var2("var2", 42);
  std::cout << "var2 value is " << var2() << std::endl;
  EraserObject<std::string> var3("var3");
  std::cout << "var3 value is " << var3() << std::endl;
  var3 = "foo";
  std::cout << "var3 value is " << var3() << std::endl;
  current_logger->show();

  ck_assert(util::check_warnings(prefix + "uninit_access.txt"));
}
END_TEST

struct S {
public:
  EraserObject<uint64_t> secret;
  S() : secret("secret", 4242){};
  void plus();
  void minus();
};

void S::plus() { secret = secret() + 42; }

void S::minus() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  secret = secret() - 42;
}

START_TEST(unprot_access) {
  using namespace eraser;
  current_logger->clear();
  S s;
  std::thread t1(&S::plus, std::ref(s));
  std::thread t2(&S::minus, std::ref(s));
  t1.join();
  t2.join();
  std::cout << "s.secret is " << s.secret() << std::endl;
  current_logger->show();
  ck_assert(util::check_warnings(prefix + "unprot_access.txt"));
}
END_TEST

int main(int argc, char *argv[]) {
  eraser::set_logger(new Logger());

  Suite *suite_s = suite_create("BasicTests");
  TCase *tcase_s = tcase_create("Simple");
  tcase_add_test(tcase_s, sanity_check);
  tcase_add_test(tcase_s, uninit_access);
  tcase_add_test(tcase_s, unprot_access);
  suite_add_tcase(suite_s, tcase_s);

  Suite *suite_a = suite_create("BasicTests");
  TCase *tcase_a = tcase_create("Advanced");
  tcase_add_test(tcase_a, sanity_check);
  tcase_add_test(tcase_a, uninit_access);
  tcase_add_test(tcase_a, unprot_access);
  suite_add_tcase(suite_a, tcase_a);

  int number_failed = 0;
  SRunner *suite_runner = nullptr;

  suite_runner = srunner_create(suite_s);
  if (argc == 3)
    srunner_set_log(suite_runner, argv[1]);

  prefix = argv[0];
  prefix += "_s/";
  eraser::set_algorithm(new SimpleEraser());
  srunner_run_all(suite_runner, CK_NORMAL);
  number_failed += srunner_ntests_failed(suite_runner);
  std::cout << "Finished BasicTests for SimpleEraser" << std::endl;

  srunner_free(suite_runner);

  suite_runner = srunner_create(suite_a);
  if (argc == 3)
    srunner_set_log(suite_runner, argv[2]);

  prefix = argv[0];
  prefix += "_a/";
  eraser::set_algorithm(new AdvancedEraser());
  srunner_run_all(suite_runner, CK_NORMAL);
  number_failed += srunner_ntests_failed(suite_runner);
  std::cout << "Finished BasicTests for AdvancedEraser" << std::endl;

  srunner_free(suite_runner);

  eraser::clear_algorithm();
  eraser::clear_logger();

  return !number_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
