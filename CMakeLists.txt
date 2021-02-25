cmake_minimum_required(VERSION 3.12)
project("my_ldd" LANGUAGES C CXX VERSION "0.1")

add_executable(my_ldd
    ./src/main.cpp
)

target_compile_options(my_ldd PRIVATE -Wall -Wextra -pedantic)
target_compile_features(my_ldd PRIVATE cxx_std_17)
