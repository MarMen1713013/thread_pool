cmake_minimum_required(VERSION 3.22.2)
project(thread_pool VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(Threads REQUIRED)
add_library(thread_safe src/thread_safe.cpp)
target_include_directories(thread_safe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(thread_safe PUBLIC pthread)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(src/example)
    add_subdirectory(src/tests)
endif()
