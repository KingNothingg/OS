cmake_minimum_required(VERSION 3.17)
project(OS5)

set(CMAKE_C_STANDARD 99)

add_executable(staticMain source/staticMain.c)
add_executable(dynamicMain source/dynamicMain.c)

add_library(first SHARED source/realization1.c)
set_target_properties(first PROPERTIES PUBLIC_HEADER source/sharedRealization.c)
add_library(second SHARED source/realization2.c)
set_target_properties(second PROPERTIES PUBLIC_HEADER source/sharedRealization.c)

target_link_libraries(first m)
target_link_libraries(second m)

set(custom_targets)
set(lab_link_type "first" CACHE STRING "User-specified link option")

list(APPEND custom_targets first)
list(APPEND custom_targets second)

message("Link library for staticMain will be now " ${lab_link_type})

target_link_libraries(staticMain ${lab_link_type})
target_link_libraries(dynamicMain ${CMAKE_DL_LIBS})
add_dependencies(dynamicMain first second)

