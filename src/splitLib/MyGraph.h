#ifndef MYPOLYGON_MYGRAPH_H
#define MYPOLYGON_MYGRAPH_H

#include "splitter.h"
#include <unordered_map>


class Node {
    Node* _edges[4] = {nullptr};

    int _direction_to_idx(const Point&, const Point&);
public:
    const Point position;

    Node(const Point& p) : position(p){};
    void add_edge(Node*);
    Node* get_next_node(Node*);
};


struct PointHasher {
    std::size_t operator()(const Point& p) const {
        return (std::hash<int>()(p.x()) << 1)
             ^ (std::hash<int>()(p.y()));
    }
};

class MyGraph {
    unordered_map<Point, Node*, PointHasher> _node_map;
    set<Segment> _edge_set;

    vector<Point> _get_subregion(const Segment&);
public:
    void add_edge(const Point& p1, const Point& p2);
    void add_chord(const Segment&);
    vector<vector<Point>> get_subregions();
};

#endif //MYPOLYGON_MYGRAPH_H
