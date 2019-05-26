#include "gtest/gtest.h"
#include "splitLib/splitter.h"
#include "splitLib/BipGraph.h"
#include "splitLib/MyGraph.h"


TEST(splitLib_test, hopcroftKarp) {
    BipGraph graph(5, 5);
    graph.addEdge(1, 1);
    graph.addEdge(2, 2);
    graph.addEdge(3, 3);
    graph.addEdge(4, 4);
    graph.addEdge(5, 5);

    int match_num = graph.hopcroftKarp();
    EXPECT_EQ(match_num, 5);

    graph.maximum_independent_set();
    EXPECT_EQ(graph.U_ind.size() + graph.V_ind.size(), 5);
}


TEST(splitLib_test, Mygraph_trivial) {
    MyGraph graph;
    EXPECT_EQ(graph.get_subregions().size(), 0);

    graph.add_edge(Point(0, 0), Point(0, 1));
    graph.add_edge(Point(0, 1), Point(1, 1));
    graph.add_edge(Point(1, 1), Point(1, 0));
    graph.add_edge(Point(1, 0), Point(0, 0));
    EXPECT_EQ(graph.get_subregions().size(), 1);
}


TEST(splitLib_test, Mygraph_with_chord) {
    MyGraph graph;
    graph.add_edge(Point(0, 0), Point(0, 100));
    graph.add_edge(Point(0, 100), Point(50, 100));
    graph.add_edge(Point(50, 100), Point(50, 120));
    graph.add_edge(Point(50, 120), Point(100, 120));
    graph.add_edge(Point(100, 120), Point(100, -20));
    graph.add_edge(Point(100, -20), Point(50, -20));
    graph.add_edge(Point(50, -20), Point(50, 0));
    graph.add_edge(Point(50, 0), Point(0, 0));

    graph.add_chord(make_pair(Point(50, 100), Point(50, 0)));
    const auto& subregions = graph.get_subregions();
    EXPECT_EQ(subregions.size(), 2);
}
