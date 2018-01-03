cmake_minimum_required(VERSION 3.5)
project(linked_list CXX)

add_executable(linked_list 
    main.cpp
    singly_linked_list.test.cpp
    singly_linked_list.hpp)

target_compile_options(linked_list
    PUBLIC 
    -g2 -fconcepts -Wall -Wextra -Werror -fsanitize=address)
target_link_libraries(linked_list PUBLIC -fsanitize=address)

target_include_directories(linked_list PRIVATE ${CMAKE_CURRENT_LIST_DIR}/..)
set_target_properties(linked_list PROPERTIES CXX_STANDARD 17)

find_package(Catch REQUIRED)
target_link_libraries(linked_list PRIVATE Catch::Catch)

enable_testing()
add_test(tests linked_list)