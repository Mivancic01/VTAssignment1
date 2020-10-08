#include "../Eraser.h"
#include "../AdvancedEraser.h"

#include <inttypes.h>
#include <map>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <pthread.h>

using namespace eraser;

void init() __attribute__ ((constructor));

void die()
{
  std::cerr << eraser::current_logger->get();
  eraser::clear_logger();
  eraser::clear_algorithm();
}

void init()
{
  eraser::set_algorithm(new AdvancedEraser());
  eraser::set_logger(new Logger());
  std::atexit(die);
}

void report_load(void *addr, size_t size, void *ip) {
  /*
  * \todo Call the appropriate function in your eraser algorithm
  *  to report reads for each accessed byte.
  * Note: use an empty string as the objname
  * to tell the algorithm that no name is available.
  */
}

void report_store(void *addr, size_t size, void *ip) {
  /*
  * \todo Call the appropriate function in your eraser algorithm
  *  to report writes for each accessed byte.
  * Note: use an empty string as the objname
  * to tell the algorithm that no name is available.
  */
}

int pmutex_lock(pthread_mutex_t* mux)
{
  /*
  * \todo First a acquire the lock using pthread_mutex_lock.
  * \todo Then report this to your eraser algorithm.
  * The return value should be the same as the one from pthread_mutex_lock.
  */
  return 0;
}

int pmutex_unlock(pthread_mutex_t* mux)
{
  /*
  * \todo First report this to your eraser algorithm. 
  * \todo Then release the lock using pthread_mutex_unlock.
  * The return value should be the same as the one from pthread_mutex_unlock.
  */
  return 0;
}

void mutex_lock(std::mutex *mux) {
  pmutex_lock((mux->native_handle()));
}

void mutex_unlock(std::mutex *mux) {
  pmutex_unlock((mux->native_handle()));
}



