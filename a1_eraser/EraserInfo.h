#ifndef ERASER_ERASERINFO_H
#define ERASER_ERASERINFO_H

#include "types.h"
#include <thread>

namespace eraser {
class AdvancedEraser;

// Class used to track object information for advanced eraser
struct EraserInfo {
public:
  enum ObjectState { VIRGIN, EXCLUSIVE, SHARED, SHARED_MODIFIED };

  EraserInfo();
  ~EraserInfo();
  //\todo Modify this class to track all relevant object information.
  //\todo Do not change any of the existing member signatures.
  friend class AdvancedEraser;
};

} // namespace eraser

#endif // ERASER_ERASERINFO_H
