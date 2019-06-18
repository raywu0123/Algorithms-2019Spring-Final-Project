cmake_minimum_required(VERSION 3.7)
project(myPolygon_project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")

include_directories(src)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

