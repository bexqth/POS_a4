cmake_minimum_required(VERSION 3.18)
project(uloha4 C)

set(CMAKE_C_STANDARD 11)

add_executable(uloha4 main.c)
target_link_libraries(uloha4 pthread)