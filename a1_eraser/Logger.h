#ifndef ERASER_LOGGER_H
#define ERASER_LOGGER_H

#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <iostream>

namespace eraser {
class Logger {
public:
  Logger();
  inline void uninitialisedRead(std::thread::id id, const void *object,
                                const void *ip, const std::string &objname);

  inline void unprotectedRead(std::thread::id id, const void *object,
                              const void *ip, const std::string &objname);

  inline void unprotectedWrite(std::thread::id id, const void *object,
                               const void *ip, const std::string &objname);

  inline void clear();

  inline void show();

  inline std::string get();

  inline void assumeInit() { assume_initd_ = true; };

  inline void assumeUninit() { assume_initd_ = false; };

private:
  uint64_t getThreadId(std::thread::id tid);

  void warning(const char *type, std::thread::id tid, const void *object,
               const void *ip, const std::string &objname);

  std::string log_;
  std::mutex log_mutex_;
  bool assume_initd_;

  std::unordered_map<std::thread::id, uint64_t> threads_;
  uint64_t thread_count_;
  std::mutex threads_mutex_;
};

void Logger::uninitialisedRead(std::thread::id id, const void *object,
                               const void *ip, const std::string &objname) {
  if (assume_initd_)
    return;
  warning("Uninitialised read from", id, object, ip, objname);
}

void Logger::unprotectedRead(std::thread::id id, const void *object,
                             const void *ip, const std::string &objname) {
  warning("Unprotected read from", id, object, ip, objname);
}

void Logger::unprotectedWrite(std::thread::id id, const void *object,
                              const void *ip, const std::string &objname) {
  warning("Unprotected write to", id, object, ip, objname);
}

void Logger::clear() {
  std::unique_lock<std::mutex> lock(log_mutex_);
  log_.clear();
}

void Logger::show() {
  std::unique_lock<std::mutex> lock(log_mutex_);
  std::cout << log_ << std::endl;
}

std::string Logger::get() {
  std::unique_lock<std::mutex> lock(log_mutex_);
  return log_;
}

extern Logger *current_logger;
extern void set_logger(Logger *log);
extern void clear_logger();
} // namespace eraser
#endif // ERASER_LOGGER_H
