#ifndef ERASER_ERASERALGORITHM_H
#define ERASER_ERASERALGORITHM_H

#include <mutex>
#include <thread>
#include <unordered_map>

#include "Logger.h"
#include "types.h"

namespace eraser {
// For a reference on how the simple eraser algorithm works please refer to the
// lecture slides [1], the original eraser paper [2], and the assignment
// description. [1]
// https://teaching.iaik.tugraz.at/_media/vt/vt02_-_concurrency.pdf [2] Eraser:
// S. Savage, M. Burrows, G. Nelson, P. Sobalvarro, T. Anderson, Eraser:A
// Dynamic Race Detector for Multithreaded Programs, ACM Transactions on
// Computer Systems Volume 15 Issue 4, November 1997.
// http://www.cs.ucsd.edu/users/savage/papers/Tocs97.pdf
class Eraser {
public:
  Eraser();
  virtual ~Eraser();
  virtual void lockCalled(std::thread::id id, const void *lock);
  virtual void unlockCalled(std::thread::id id, const void *lock);
  virtual void objectRead(std::thread::id id, void *object,
                          const void *ip, const std::string &objname) = 0;
  virtual void objectWrite(std::thread::id id, void *object,
                           const void *ip, const std::string &objname) = 0;

protected:
  //\todo Modify this class and/or its derived classes to track relevant object
  //      and lock information. Do not change any of the existing member
  //      signatures.
  std::mutex eraser_mutex_;
};

extern Eraser *current_algorithm;

extern void set_algorithm(Eraser *algo);
extern void clear_algorithm();
} // namespace eraser

#endif // ERASER_ERASERALGORITHM_H
