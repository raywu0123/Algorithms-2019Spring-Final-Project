cmake_minimum_required(VERSION 3.14)
project(myPolygon)


set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-O3")

add_library(Design Design.cpp Design.h)
add_library(Parser parser.cpp parser.h)
add_library(Operation Operation.cpp Operation.h)
add_library(bLib bLib/bLibShape.h bLib/bLibPtr.h)

add_executable(myPolygon main.cpp parser.h Design.h)

set_target_properties(bLib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(myPolygon Design Parser Operation bLib)