# - Try to find Check
# Once done this will define
#  CHECK_FOUND - System has check
#  CHECK_INCLUDE_DIRS - The check include directories
#  CHECK_LIBRARIES - The libraries needed to use check
#  CHECK_DEFINITIONS - Compiler switches required for using check

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_CHECK QUIET check)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CHECK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Check DEFAULT_MSG
        PC_CHECK_LIBRARIES)

mark_as_advanced(CHECK_INCLUDE_DIR CHECK_LIBRARY)

set(CHECK_DEFINITIONS ${PC_CHECK_CFLAGS_OTHER})
set(CHECK_INCLUDE_DIRS ${PC_CHECK_INCLUDE_DIRS})
set(CHECK_LIBRARY_DIRS ${PC_CHECK_LIBRARY_DIRS})
set(CHECK_LIBRARIES ${PC_CHECK_LIBRARIES})
