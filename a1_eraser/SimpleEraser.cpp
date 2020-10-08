#include "SimpleEraser.h"

using namespace eraser;



void SimpleEraser::objectRead(std::thread::id id, void *object,
                              const void *ip, const std::string &objname) {
  //\todo Implement the simple eraser algorithm for the case when an object is
  // read. \todo Make sure to call current_logger->uninitialisedRead and
  // current_logger->unprotectedRead \todo whenever you detect undefined
  // behaviour or race conditions.
}

void SimpleEraser::objectWrite(std::thread::id id, void *object,
                               const void *ip, const std::string &objname) {
  //\todo Implement the simple eraser algorithm for the case when an object is
  // written. \todo Make sure to call current_logger->unprotectedWrite whenever
  // you detect race conditions.
}