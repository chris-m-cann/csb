cmake_minimum_required(VERSION 3.5)
project(csb CXX)

add_executable(csbexe
        main.cpp
        static_array/static_array.test.cpp
        linked_list/singly_linked_list.test.cpp
        binary_tree/binary_tree.test.cpp reverse/reverse.test.cpp binary_tree/tree_utils_test.cpp red_black_tree/red_black_tree.test.cpp)

target_compile_options(csbexe PUBLIC -g2 -Wall -Wextra -Werror -fsanitize=address)

target_link_libraries(csbexe PUBLIC -fsanitize=address)

target_include_directories(csbexe
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        #$ < INSTALL_INTERFACE : include / mylib > # < prefix > / include / mylib
)

set_target_properties(csbexe PROPERTIES CXX_STANDARD 17)

find_package(Catch2 REQUIRED)
target_link_libraries(csbexe PRIVATE Catch2::Catch2)

#ToDo: if your going to use GSL then make it visible in the cmake dependencies
#find_package(MicrosoftGSL REQUIRED)
#target_link_libraries(csbexe PRIVATE MicrosoftGSL::gsl)

