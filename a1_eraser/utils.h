#ifndef ERASER_UTILS_H
#define ERASER_UTILS_H
#include <string>

// return debugging information about the address ip
std::string debug_info(const void *ip);

// given a mangled c++ function symbol name, returns the original name
std::string demangle(const char *function_name);

// returns the instruction pointer of the caller
const void *get_caller_ip();

#endif // ERASER_UTILS_H
