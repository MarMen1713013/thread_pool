cmake_minimum_required(VERSION 3.22.2)
project(thread_pool VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
add_library(thread_safe thread_safe.cpp)
#message(${Threads})
target_link_libraries(thread_safe PUBLIC pthread)

add_subdirectory(example)
add_subdirectory(tests)