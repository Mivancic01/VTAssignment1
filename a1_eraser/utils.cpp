#include "utils.h"
#include <assert.h>
#include <cxxabi.h>
#include <elfutils/libdwfl.h>
#include <libunwind.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>

std::string debug_info(const void *ip) {
  std::string result;

  char *debuginfo_path = nullptr;
  Dwfl_Callbacks callbacks = {.find_elf = dwfl_linux_proc_find_elf,
                              .find_debuginfo = dwfl_standard_find_debuginfo,
                              .section_address = nullptr,
                              .debuginfo_path = &debuginfo_path};

  Dwfl *dwfl = dwfl_begin(&callbacks);
  assert(dwfl != nullptr);

  assert(dwfl_linux_proc_report(dwfl, getpid()) == 0);
  assert(dwfl_report_end(dwfl, nullptr, nullptr) == 0);

  auto addr = (uintptr_t)ip;
  Dwfl_Module *module = dwfl_addrmodule(dwfl, addr);
  const char *function_name = dwfl_module_addrname(module, addr);

  result += "function " + demangle(function_name);
  result += " at ";
  char rest[256];
  Dwfl_Line *line = dwfl_getsrc(dwfl, addr);
  if (line != nullptr) {
    int nline;
    Dwarf_Addr addr_;
    const char *filename =
        dwfl_lineinfo(line, &addr_, &nline, nullptr, nullptr, nullptr);
    snprintf(rest, 256, "%s:%d", strrchr(filename, '/') + 1, nline);
  } else
    snprintf(rest, 256, "%p", ip);
  result += rest;
  dwfl_end(dwfl);
  return result;
}

std::string demangle(const char *function_name) {
  std::string result;
  int status;
  char *demangled =
      abi::__cxa_demangle(function_name, nullptr, nullptr, &status);
  if (status == 0)
    result = demangled;
  else
    result = function_name;

  std::free(demangled);
  return result;
}

// adapted backtrace implementation taken from:
// https://eli.thegreenplace.net/2015/programmatic-access-to-the-call-stack-in-c/
const void *get_caller_ip() {
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding.
  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  // Go up the stack twice to get the callers caller
  int num_scopes = 0;
  num_scopes = unw_step(&cursor);
  assert(num_scopes > 0);
  num_scopes = unw_step(&cursor);
  assert(num_scopes > 0);

  unw_word_t ip;
  unw_get_reg(&cursor, UNW_REG_IP, &ip);

  return (const void *)(ip - 4);
}