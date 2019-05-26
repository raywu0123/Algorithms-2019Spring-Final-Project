cmake_minimum_required(VERSION 3.7)
project(myPolygon_project)


include_directories(src)
add_subdirectory(src)


enable_testing()
add_subdirectory(tests)

