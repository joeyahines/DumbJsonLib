cmake_minimum_required(VERSION 3.13)
project(json_lib C)

set(CMAKE_C_STANDARD 99)

add_library(json_lib json_lib.c json_lib.h)

add_executable(test  test/CuTest.c test/CuTest.h json_lib.c json_lib.h test/tests.c)
