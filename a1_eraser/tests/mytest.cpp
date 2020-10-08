#include "../AdvancedEraser.h"
#include "../EraserLock.h"
#include "../EraserObject.h"
#include "../SimpleEraser.h"

#include <iostream>
#include <algorithm>
#include <check.h>

#include <regex>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

#include "util.h"

////////////////////////////////////////////////////

START_TEST(simple_warn_write) {
  using namespace eraser;
  current_logger->clear();
  //\todo your testcase code
  current_logger->show();
}
END_TEST

////////////////////////////////////////////////////

START_TEST(simple_warn_read) {
  using namespace eraser;
  current_logger->clear();
  //\todo your testcase code
  current_logger->show();
}
END_TEST

////////////////////////////////////////////////////

void f(EraserObject<int> &var) {
  //\hint you can overwrite this function
}

START_TEST(simple_warn_lock) {
  using namespace eraser;
  current_logger->clear();
  EraserLock l("lock");
  l.lock();
  EraserObject<int> i("var", 0);
  //\todo your testcase code
  l.unlock();
  current_logger->show();
}
END_TEST

////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  eraser::set_logger(new Logger());

  Suite *suite_s = suite_create("MySimpleTests");
  TCase *tcase_s = tcase_create("SimpleTests");
  tcase_add_test(tcase_s, simple_warn_write);
  tcase_add_test(tcase_s, simple_warn_read);
  tcase_add_test(tcase_s, simple_warn_lock);

  suite_add_tcase(suite_s, tcase_s);

  Suite *suite_a = suite_create("MyAdvancedTests");
  TCase *tcase_a = tcase_create("AdvancedTests");
  tcase_add_test(tcase_a, simple_warn_write);
  tcase_add_test(tcase_a, simple_warn_read);
  tcase_add_test(tcase_a, simple_warn_lock);

  suite_add_tcase(suite_a, tcase_a);

  int number_failed = 0;
  SRunner *suite_runner = nullptr;

  suite_runner = srunner_create(suite_s);
  if (argc == 3)
    srunner_set_log(suite_runner, argv[1]);

  eraser::set_algorithm(new SimpleEraser());
  srunner_run(suite_runner, "MySimpleTests", "SimpleTests", CK_NORMAL);
  number_failed += srunner_ntests_failed(suite_runner);
  std::cout << "Finished MyTests for SimpleEraser" << std::endl;

  srunner_free(suite_runner);

  suite_runner = srunner_create(suite_a);
  if (argc == 3)
    srunner_set_log(suite_runner, argv[2]);

  eraser::set_algorithm(new AdvancedEraser());
  srunner_run(suite_runner, "MyAdvancedTests", "AdvancedTests", CK_NORMAL);
  number_failed += srunner_ntests_failed(suite_runner);
  std::cout << "Finished MyTests for AdvancedEraser" << std::endl;

  srunner_free(suite_runner);

  eraser::clear_algorithm();
  eraser::clear_logger();

  return !number_failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
