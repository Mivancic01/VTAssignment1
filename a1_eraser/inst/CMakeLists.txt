find_package(Threads)

get_filename_component(CEMILA_CXX_COMPILER cemila.py ABSOLUTE)
set(CEMILA_CXX_OPTIONS -pthread -O0 -g -Wall -Wextra -c)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES 1)
foreach (target IN ITEMS unprot_access prot_access my_instr_test)
    set(target_obj ${CMAKE_CURRENT_BINARY_DIR}/inst_${target}.o)
    set(target_cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/${target}.cpp)
    # if(FALSE)
    add_custom_target(inst_${target}.o
            COMMAND ${CEMILA_CXX_COMPILER} ${CEMILA_CXX_OPTIONS} ${target_cpp} -o ${target_obj}
            SOURCES ${target_cpp}
            BYPRODUCTS ${target_obj}
            COMMENT "Building ${target_obj}")
    add_executable(inst_${target}.elf inst_${target}.o cemila.cpp)
    add_dependencies(inst_${target}.elf inst_${target}.o)
    set_target_properties(inst_${target}.elf PROPERTIES LINKER_LANGUAGE CXX)
    set_source_files_properties(
            inst_${target}.o
            PROPERTIES
            EXTERNAL_OBJECT true
            GENERATED true)
    # endif()
    # add_executable(inst_${target}.elf ${target_cpp} cemila.cpp)
    target_link_libraries(inst_${target}.elf eraser)
endforeach (target)
