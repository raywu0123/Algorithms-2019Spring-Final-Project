include(CTest)
include_directories(${GTEST_SOURCE_DIR}/include ${GTEST_SOURCE_DIR})

add_executable(runSplitLibTests splitLibTest.cpp)

target_link_libraries(runSplitLibTests gtest gtest_main)
target_link_libraries(runSplitLibTests myPolygonLib)

add_test(
        NAME splitLibTest
        COMMAND runSplitLibTests
)
