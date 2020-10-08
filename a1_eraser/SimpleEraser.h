#ifndef ERASER_SIMPLEERASER_H
#define ERASER_SIMPLEERASER_H

#include "Eraser.h"

namespace eraser {
class SimpleEraser : public Eraser {
public:
  void objectRead(std::thread::id id, void *object, const void *ip,
                  const std::string &objname) override;
  void objectWrite(std::thread::id id, void *object, const void *ip,
                   const std::string &objname) override;
};

} // namespace eraser

#endif // ERASER_SIMPLEERASER_H
