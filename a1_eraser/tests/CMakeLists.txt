find_package(Check REQUIRED)
add_definitions(${CHECK_DEFINITIONS})
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

include(configure_files)

foreach (target IN ITEMS basic mytest)
    add_executable("test_${target}" "${target}.cpp" util.h util.cpp)
    target_link_libraries("test_${target}" eraser ${CHECK_LIBRARIES})
    configure_files(${CMAKE_CURRENT_SOURCE_DIR}/test_basic_a ${CMAKE_CURRENT_BINARY_DIR}/test_basic_a)
    configure_files(${CMAKE_CURRENT_SOURCE_DIR}/test_basic_s ${CMAKE_CURRENT_BINARY_DIR}/test_basic_s)

    add_test(NAME ${target}
            COMMAND $<TARGET_FILE:test_${target}> "${CMAKE_CURRENT_BINARY_DIR}/${target}_s.log" "${CMAKE_CURRENT_BINARY_DIR}/${target}_a.log"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    add_custom_target("valgrind_test_${target}"
            env CK_FORK=no valgrind ${VALGRIND_ARGUMENTS}
            $<TARGET_FILE:test_${target}>
            DEPENDS test_${target}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endforeach (target)
