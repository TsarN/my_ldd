cmake_minimum_required(VERSION 3.12)
project("my_ldd" LANGUAGES C CXX VERSION "0.1")
include(cmake/CPM.cmake)

add_executable(my_ldd
    ./src/main.cpp
        src/dependencies.cpp src/dependencies.h)

target_compile_options(my_ldd PRIVATE -Wall -Wextra -pedantic)
target_compile_features(my_ldd PRIVATE cxx_std_20)

CPMAddPackage("gh:serge1/ELFIO#57e614a486d6dc45a23d92383c7002ea613e08bf")
target_link_libraries(my_ldd elfio::elfio)

add_subdirectory(example)
