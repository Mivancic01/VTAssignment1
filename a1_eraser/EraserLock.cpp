#include "EraserLock.h"
#include "Eraser.h"

using namespace eraser;

EraserLock::EraserLock(const std::string &name) : lock_(), name_(name) {}

void EraserLock::lock() {
  lock_.lock();
  current_algorithm->lockCalled(std::this_thread::get_id(), &lock_);
}

void EraserLock::unlock() {
  current_algorithm->unlockCalled(std::this_thread::get_id(), &lock_);
  lock_.unlock();
}