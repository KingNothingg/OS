cmake_minimum_required(VERSION 3.17)
project(OS4ISLOST C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(child child.c)
add_executable(main main.c)
target_link_libraries(main PRIVATE Threads::Threads rt)
target_link_libraries(child PRIVATE Threads::Threads rt)

add_dependencies(main child)