project(myPolygon)


set(HEADER_FILES
        Design.h
        parser.h
        Operation.h
        splitLib/splitter.h
        splitLib/sweep_plane.h
        splitLib/BipGraph.h
        splitLib/MyGraph.h
        tqdm/tqdm.h
        )

set(SOURCE_FILES
        Design.cpp
        parser.cpp
        Operation.cpp
        splitLib/splitter.cpp
        splitLib/BipGraph.cpp
        splitLib/MyGraph.cpp
        )

add_library(myPolygonLib STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_executable(myPolygon main.cpp)
target_link_libraries(myPolygon myPolygonLib)