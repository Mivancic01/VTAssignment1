#ifndef ERASER_TYPES_H
#define ERASER_TYPES_H

#include <mutex>
#include <unordered_set>

//\hint This is a very useful class for tracking sets of std::mutex pointers
typedef std::unordered_set<const void *> mutex_pset;


#endif // ERASER_TYPES_H
