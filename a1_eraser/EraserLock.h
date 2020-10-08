#ifndef ERASER_ERASERLOCK_H
#define ERASER_ERASERLOCK_H

#include <mutex>
#include <string>

namespace eraser {

class Eraser;
// Simple eraser wrapper for a std::mutex object
class EraserLock {
public:
  EraserLock(const std::string &name);

  void lock();

  void unlock();

  operator std::string() const { return name_; };

private:
  std::mutex lock_;
  std::string name_;
};

} // namespace eraser
#endif // ERASER_ERASERLOCK_H
