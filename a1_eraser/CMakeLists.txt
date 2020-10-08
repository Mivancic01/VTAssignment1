cmake_minimum_required(VERSION 3.7)
project(eraser)
enable_testing()

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
add_definitions(-DREFERENCE)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g -Wall -Wextra ${LIBUNWIND_DEFINITIONS})

set(SOURCE_FILES Eraser.cpp EraserLock.cpp SimpleEraser.cpp AdvancedEraser.cpp EraserInfo.cpp Logger.cpp utils.cpp)
add_library(eraser SHARED ${SOURCE_FILES})
target_compile_options(eraser PUBLIC -O1)

find_package(libunwind REQUIRED)
find_package(DWARF REQUIRED)
find_package(Threads)
find_package(LibLZMA)
target_link_libraries(eraser ${LIBUNWIND_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBLZMA_LIBRARIES} ${DWARF_LIBRARY})

add_subdirectory(tests)
add_subdirectory(inst)
