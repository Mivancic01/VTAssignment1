#include "util.h"
#include "../Eraser.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

static void load_regex(std::vector<std::regex> &regexes, std::string filename) {
  // std::cout << "reading file: " << filename << std::endl;
  std::ifstream input(filename);
  std::string line;
  while (std::getline(input, line, '\n')) {
    // std::cout << "line: " << line << std::endl;
    regexes.emplace_back(line);
  }
}

bool util::check_correct() {
  using namespace eraser;
  return current_logger->get() == "";
}

bool util::check_warnings(std::string filename) {
  std::vector<std::regex> re_checks;
  load_regex(re_checks, filename);
  using namespace eraser;
  std::stringstream ss(current_logger->get());
  std::string line;
  auto re_iter = re_checks.begin();
  while (true) {
    bool c = !std::getline(ss, line, '\n');
    if (c && re_iter == re_checks.end())
      return true;
    if (c)
      return false;
    if (re_iter == re_checks.end())
      return false;
    if (!std::regex_match(line, *re_iter))
      return false;
    re_iter++;
  }
}
