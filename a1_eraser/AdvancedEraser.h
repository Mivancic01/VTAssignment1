#ifndef ERASER_ADVANCEDERASER_H
#define ERASER_ADVANCEDERASER_H

#include "Eraser.h"
#include "EraserInfo.h"

namespace eraser {
class AdvancedEraser : public Eraser {
public:
  void objectRead(std::thread::id id, void *object, const void *ip,
                  const std::string &objname) override;

  void objectWrite(std::thread::id id, void *object, const void *ip,
                   const std::string &objname) override;
};
} // namespace eraser

#endif // ERASER_ADVANCEDERASER_H
