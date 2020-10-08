#include "Logger.h"
#include "utils.h"
#include <assert.h>
#include <sstream>

using namespace eraser;

Logger *eraser::current_logger = nullptr;

Logger::Logger() : assume_initd_(false), thread_count_(0) {}

void Logger::warning(const char *type, std::thread::id tid, const void *object,
                     const void *ip, const std::string &objname) {
  assert(type != nullptr);
  uint64_t id = getThreadId(tid);
  std::string info = debug_info(ip);
  std::unique_lock<std::mutex> lock(log_mutex_);
  log_ += "Thread " + std::to_string(id) + ": ";
  log_ += type; log_ += " ";
  if(objname.empty())
  {
    std::stringstream ss;
    ss << "(" << object << ")";
    log_ += ss.str();
  }
  else
    log_ += "(" + objname + ")";
  log_ += " in " + info + "\n";
}

uint64_t Logger::getThreadId(std::thread::id tid) {
  std::unique_lock<std::mutex> sync_lock(threads_mutex_);
  uint64_t id = threads_.emplace(tid, thread_count_).first->second;
  if (thread_count_ == id)
    thread_count_++;
  // std::printf("Thread_count: %ld\n", thread_count_);
  return id;
}

void eraser::set_logger(Logger *log) {
  delete current_logger;
  current_logger = log;
}

void eraser::clear_logger() {
  delete current_logger;
  current_logger = nullptr;
}