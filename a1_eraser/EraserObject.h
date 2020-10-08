#ifndef ERASER_ERASEROBJECT_H
#define ERASER_ERASEROBJECT_H

#include <mutex>
#include <string>

#include "Eraser.h"

namespace eraser {
// Simple template class for object wrappers
template <class Type> class EraserObject {
public:
  template <class... Args>
  EraserObject(const std::string &name, Args &&... args);

  // Object setter
  void operator=(const Type &other);

  // Object getter
  const Type &operator()();

private:
  Type object_;
  std::string name_;
  std::mutex object_access_;
};

} // namespace eraser

/// Template implementation
#include "EraserObject.tpp"

#endif // ERASER_ERASEROBJECT_H
