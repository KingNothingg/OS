cmake_minimum_required(VERSION 3.10)
project(OS678)

set(ZMQ_INCLUDE_DIR /usr/local/include)

find_library(cZeroMQ NAMES zmq PATHS /usr/local/lib)
find_library(ZeroMQ NAMES zmqpp PATHS /usr/local/lib)

add_executable(main main.cpp BTree.hpp server.hpp)
add_executable(child child.cpp BTree.hpp server.hpp)

target_include_directories(main PUBLIC ${ZMQ_INCLUDE_DIR})
target_include_directories(child PUBLIC ${ZMQ_INCLUDE_DIR})

target_link_libraries(main ${ZeroMQ} ${cZeroMQ})
target_link_libraries(child ${ZeroMQ} ${cZeroMQ})