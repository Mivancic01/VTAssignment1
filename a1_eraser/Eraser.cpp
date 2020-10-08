#include "Eraser.h"
#include <assert.h>

using namespace eraser;

Eraser *eraser::current_algorithm = nullptr;

Eraser::Eraser() :
  //\todo Define members if necessary
  eraser_mutex_()
{ }

Eraser::~Eraser()
{
  //\todo Destroy dynamic members if necessary
}



void Eraser::lockCalled(std::thread::id id, const void *lock) {
  //\todo handle EraserLock::lock()
}

void Eraser::unlockCalled(std::thread::id id, const void *lock) {
  //\todo handle EraserLock::unlock()
}


void eraser::set_algorithm(Eraser *algo) {
  delete current_algorithm;
  current_algorithm = algo;
}

void eraser::clear_algorithm() {
  delete current_algorithm;
  current_algorithm = nullptr;
}
