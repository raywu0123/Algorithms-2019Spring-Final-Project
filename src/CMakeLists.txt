cmake_minimum_required(VERSION 3.7)
project(myPolygon)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g")

add_library(Design Design.cpp Design.h)
add_library(Parser parser.cpp parser.h)
add_library(Operation Operation.cpp Operation.h)
add_library(splitLib splitLib/splitter.cpp splitLib/splitter.h)
add_library(bLib bLib/bLibShape.h bLib/bLibPtr.h)
add_library(tqdm tqdm/tqdm.h)

add_executable(myPolygon main.cpp parser.h Design.h)

set_target_properties(bLib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(tqdm PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(splitLib PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(myPolygon Design Parser Operation bLib tqdm splitLib)

