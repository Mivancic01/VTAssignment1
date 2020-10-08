#include "AdvancedEraser.h"
#include <assert.h>

using namespace eraser;


void AdvancedEraser::objectRead(std::thread::id id, void *object,
                                const void *ip, const std::string &objname) {
  /*\todo Implement the advanced eraser algorithm state transitions when objects
   *      are read. Make sure to call current_logger->unprotectedRead whenever
   *      you detect race conditions.
   */
}

void AdvancedEraser::objectWrite(std::thread::id id, void *object,
                                 const void *ip, const std::string &objname) {
  /*\todo Implement the advanced eraser algorithm state transitions when objects
   *      are written. Make sure to call current_logger->unprotectedRead
   *      whenever you detect race conditions.
   */
}
