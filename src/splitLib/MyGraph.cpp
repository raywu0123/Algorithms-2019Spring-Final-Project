#include "MyGraph.h"


int Node::_direction_to_idx(const Point& p1, const Point& p2) {
    if(p2.x() == p1.x()) {
        if(p2.y() > p1.y()) return 0;
        else return 2;
    } else if (p2.y() == p1.y()) {
        if(p2.x() > p1.x()) return 1;
        else return 3;
    } else cerr << "error converting direction to index";
}

void Node::add_edge(Node* other_node) {
    _edges[_direction_to_idx(position, other_node->position)] = other_node;
}


Node* Node::get_next_node(Node* previous_node) {
    const Point &previous_point = previous_node->position;
    int current_direction_idx = _direction_to_idx(previous_point, position);
    int i=0;
    for(; i<4; i++) {
        if(_edges[(current_direction_idx + i) % 4] == previous_node)
            continue;
        if(_edges[(current_direction_idx + i) % 4] != nullptr)
            break;
    }
    assert(_edges[(current_direction_idx + i) % 4] != nullptr);
    return _edges[(current_direction_idx + i) % 4];
}


void MyGraph::add_edge(const Point& start_point, const Point& end_point) {
    _edge_set.emplace(start_point, end_point);
    if(_node_map.find(start_point) == _node_map.end()) {
        Node* n1 = new Node(start_point);
        _node_map.emplace(start_point, n1);
    }
    if(_node_map.find(end_point) == _node_map.end()) {
        Node* n2 = new Node(end_point);
        _node_map.emplace(end_point, n2);
    }
    Node *n1 = _node_map[start_point], *n2 = _node_map[end_point];
    n1->add_edge(n2);
}


void MyGraph::add_chord(const Segment& s) {
    const Point &p1 = s.first, &p2 = s.second;
    add_edge(p1, p2);
    add_edge(p2, p1);
}


vector<vector<Point>> MyGraph::get_subregions() {
    vector<vector<Point>> subregions;

    while(not _edge_set.empty()) {
        const Segment& s = *_edge_set.begin();
        const vector<Point>& subregion = _get_subregion(s);
        subregions.push_back(subregion);
    }
    return subregions;
}

vector<Point> MyGraph::_get_subregion(const Segment& s) {
    vector<Point> raw_subregion;

    const Point &p1 = s.first, &p2 = s.second;
    Node *n1 = _node_map[p1], *n2 = _node_map[p2];
    Node *n_start = n1;
    vector<Segment> segments_to_erase;
    do {
        raw_subregion.push_back(n1->position);
        segments_to_erase.emplace_back(n1->position, n2->position);
        Node* next_node = n2->get_next_node(n1);
        n1 = n2;
        n2 = next_node;
    } while(n1 != n_start);

    for(const auto& erase_s: segments_to_erase)
        _edge_set.erase(erase_s);

    // reduce points so that no three consecutive points are on a line
    vector<Point> reduced_subregion;
    raw_subregion.push_back(raw_subregion[0]);
    reduced_subregion.push_back(raw_subregion[0]);
    int current_idx = 0;
    for(int i=1; i<raw_subregion.size() - 1; i++) {
        if(     raw_subregion[current_idx].x() == raw_subregion[i].x()
            and raw_subregion[i + 1].x() == raw_subregion[current_idx].x()
        )
            continue;
        else if(raw_subregion[current_idx].y() == raw_subregion[i].y()
            and raw_subregion[i + 1].y() == raw_subregion[current_idx].y()
        )
            continue;
        else {
            reduced_subregion.push_back(raw_subregion[i]);
            current_idx = i;
        }
    }
    return reduced_subregion;
}