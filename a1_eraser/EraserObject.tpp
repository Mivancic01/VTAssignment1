#include "EraserObject.h"
#include "utils.h"

#include <assert.h>
#include <mutex>
#include <stdlib.h>
#include <utility>

using namespace eraser;

template <class Type>
template <class... Args>
EraserObject<Type>::EraserObject(const std::string &name, Args &&... args)
    : object_(std::forward<Args>(args)...), name_(name), object_access_() {
  std::unique_lock<std::mutex> sync_lock(object_access_);
  if (!((std::is_fundamental<Type>::value || std::is_pointer<Type>::value) &&
        sizeof...(Args) == 0)) {
    const void *ip = get_caller_ip();
    current_algorithm->objectWrite(std::this_thread::get_id(), &object_, ip,
                                   name_);
  }
}

template <class Type> void EraserObject<Type>::operator=(const Type &other) {
  std::unique_lock<std::mutex> sync_lock(object_access_);
  const void *ip = get_caller_ip();
  current_algorithm->objectWrite(std::this_thread::get_id(), &object_, ip,
                                 name_);
  object_ = other;
}

template <class Type> const Type &EraserObject<Type>::operator()() {
  std::unique_lock<std::mutex> sync_lock(object_access_);
  const void *ip = get_caller_ip();
  current_algorithm->objectRead(std::this_thread::get_id(), &object_, ip,
                                name_);
  return object_;
}
