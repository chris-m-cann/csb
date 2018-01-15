cmake_minimum_required(VERSION 3.5)
project(static_array CXX)

add_executable(static_array 
    main.cpp
    static_array.test.cpp
    static_array.hpp)

target_compile_options(static_array
    PUBLIC 
    -g2 -fconcepts -Wall -Wextra -Werror -fsanitize=address)
target_link_libraries(static_array PUBLIC -fsanitize=address)

target_include_directories(static_array PRIVATE ${CMAKE_CURRENT_LIST_DIR}/..)
set_target_properties(static_array PROPERTIES CXX_STANDARD 17)

find_package(Catch REQUIRED)
target_link_libraries(static_array PRIVATE Catch::Catch)

enable_testing()
add_test(tests static_array)