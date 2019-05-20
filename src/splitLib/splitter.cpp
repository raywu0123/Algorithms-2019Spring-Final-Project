#include "splitter.h"
#include "sweep_plane.h"
#include "MyGraph.h"

namespace gtl = boost::polygon;

bool Comparator_XY(const pair<Point, Point>& a, const pair<Point, Point>& b) {
    if (a.first.x() != b.first.x()) return a.first.x() < b.first.x();
    return a.first.y() < b.first.y();
}

bool Comparator_YX(const pair<Point, Point>& a, const pair<Point, Point>& b) {
    if (a.first.y() != b.first.y()) return a.first.y() < b.first.y();
    return a.first.x() < b.first.x();
}


void Splitter::split(bShape* polygon) {
    HolePolygon hp = _build_graph(polygon);

    const vector<vector<Point>>& loops = _get_loops(hp);

    vector<pair<Point, Point>> H_edges, V_edges;
    _get_edges(loops, H_edges, V_edges);

    vector<pair<Point, Point>> concave_vertices = _get_concave_vertices(loops);

    vector<Segment> H_effective_chords, V_effective_chords;
    _get_effective_chords(concave_vertices, H_edges, V_edges, H_effective_chords, V_effective_chords);

    vector<Segment> H_selected_chords, V_selected_chords;
    _maximum_independent_set(H_effective_chords, V_effective_chords, H_selected_chords, V_selected_chords);

//    vector<bShape*> subregions = _dissect_by_subregions(polygon, selected_effective_chords);
//
//    vector<bBox> boxes;
//    for(int i=0; i<subregions.size(); i++) {
//        vector<bBox*>boxes_in_subregion = _split_subregion(subregions[i], concave_vertices);
//        boxes.insert(boxes.end(), boxes_in_subregion.begin(), boxes_in_subregion.end());
//    }
//    polygon->setRealBoxes(boxes);
}


HolePolygon Splitter::_build_graph(bShape* polygon) {
    const vector<bBox*>& realBoxes = polygon->m_realBoxes;
    PolygonSet ps;
    for(auto box : realBoxes) {
        ps.insert(gtl::construct<Rectangle>(box->x1(), box->y1(), box->x2(), box->y2()));
    }
    vector<HolePolygon> output;
    ps.get(output);
    return output[0];
}


vector<vector<Point>> Splitter::_get_loops(const HolePolygon& hp) {
    vector<vector<Point>> loops;
    vector<Point> exterior;
    for(const auto& v : hp)
        exterior.push_back(v);
    exterior.insert(exterior.end(), hp.begin(), std::next(hp.begin()));
    if(gtl::winding(hp) != gtl::CLOCKWISE) std::reverse(exterior.begin(), exterior.end());
    loops.push_back(exterior);

    for(auto it=hp.begin_holes(); it != hp.end_holes(); ++it) {
        const auto& hole_polygon = *it;
        vector<Point> hole;
        for(const auto & v : hole_polygon)
            hole.push_back(v);
        hole.insert(hole.end(), hole_polygon.begin(), std::next(hole_polygon.begin()));
        if(gtl::winding(hole_polygon) != gtl::COUNTERCLOCKWISE) std::reverse(hole.begin(), hole.end());
        loops.push_back(hole);
    }
    return loops;
}


void Splitter::_get_edges(const vector<vector<Point>>& loops, vector<Segment>& h_edges, vector<Segment>& v_edges) {
    for(const auto& loop : loops) {
        for(int vertice_idx=0; vertice_idx < loop.size() - 1; vertice_idx++) {
            const Point & v1 = loop[vertice_idx], & v2 = loop[vertice_idx + 1];
            if(v1.x() == v2.x()) {
                if(v1.y() < v2.y()) v_edges.emplace_back(v1, v2);
                else v_edges.emplace_back(v2, v1);
            }
            if(v1.y() == v2.y()) {
                if(v1.x() < v2.x()) h_edges.emplace_back(v1, v2);
                else h_edges.emplace_back(v2, v1);
            }
        }
    }
}


vector<pair<Point, Point>> Splitter::_get_concave_vertices(const vector<vector<Point>>& loops) {
    vector<pair<Point, Point>> concave_vertices;
    vector<vector<Point>> custom_loops = loops;
    for(auto& loop: custom_loops) loop.pop_back();
    for(const auto& loop : custom_loops) {
        for(int vertice_idx=0; vertice_idx < loop.size(); vertice_idx++) {
            int dx1 = loop[vertice_idx].x() - loop[vertice_idx - 1].x();
            int dy1 = loop[vertice_idx].y() - loop[vertice_idx - 1].y();
            int dx2 = loop[(vertice_idx + 1) % loop.size()].x() - loop[vertice_idx].x();
            int dy2 = loop[(vertice_idx + 1) % loop.size()].y() - loop[vertice_idx].y();
            if(dx1 * dy2 - dx2 * dy1 > 0) // determine concavity by outer product of edge vectors
                concave_vertices.emplace_back(loop[vertice_idx], Point(dx1, dy1));
        }
    }
    return concave_vertices;
}


void Splitter::_get_effective_chords(
    vector<pair<Point, Point>>& concave_vertices,
    const vector<Segment>& H_edges,
    const vector<Segment>& V_edges,
    vector<Segment>& H_effective_chords,
    vector<Segment>& V_effective_chords
    ) {
    vector<pair<Point, Point>> v_candidates;
    std::sort(concave_vertices.begin(), concave_vertices.end(), Comparator_XY);
    for(int i=0; i<concave_vertices.size() - 1; i++) {
        const pair<Point, Point>& v1 = concave_vertices[i];
        const pair<Point, Point>& v2 = concave_vertices[i + 1];
        if(v2.first.x() != v1.first.x()) continue;
        if( not(v1.second.y() > 0 or v1.second.x() < 0 )) continue;
        if( not(v2.second.y() < 0 or v2.second.x() > 0 )) continue;

        if(v1.first.y() < v2.first.y())
            v_candidates.emplace_back(v1.first, v2.first);
        else
            v_candidates.emplace_back(v2.first, v1.first);
    }

    vector<pair<Point, Point>> h_candidates;
    std::sort(concave_vertices.begin(), concave_vertices.end(), Comparator_YX);
    for(int i=0; i<concave_vertices.size() - 1; i++) {
        const pair<Point, Point>& v1 = concave_vertices[i];
        const pair<Point, Point>& v2 = concave_vertices[i + 1];
        if(v2.first.y() != v1.first.y()) continue;
        if( not(v1.second.x() > 0 or v1.second.y() > 0 )) continue;
        if( not(v2.second.x() < 0 or v2.second.y() < 0 )) continue;

        if(v1.first.x() < v2.first.x())
            h_candidates.emplace_back(v1.first, v2.first);
        else
            h_candidates.emplace_back(v2.first, v1.first);
    }

    vector<Event> events;
    for(auto& e: v_candidates)
        events.emplace_back(e.first.x(), PARALLEL_SEGMENT, &e);
    for(auto& e: H_edges) {
        events.emplace_back(e.first.x(), START_POINT, &e);
        events.emplace_back(e.second.x(), END_POINT, &e);
    }
    std::sort(events.begin(), events.end(), EventComparator);

    std::set<int> set;
    for(const auto& event : events) {
        if(event.type == START_POINT)
            set.emplace(event.segment->first.y());
        else if(event.type == END_POINT)
            set.erase(event.segment->first.y());
        else if(event.type == PARALLEL_SEGMENT) {
            int y1 = event.segment->first.y(), y2 = event.segment->second.y();
            auto lower_it = set.lower_bound(y1 + 1);
            auto upper_it = set.upper_bound(y2 - 1);
            // +- 1 to prevent equal case
            if((lower_it == upper_it) or (y2 - y1) <= 1) { // no intersection
                V_effective_chords.push_back(*event.segment);
            }
        }
    }

    vector<Event> events_;
    for(auto& e: h_candidates)
        events_.emplace_back(e.first.y(), PARALLEL_SEGMENT, &e);
    for(auto& e: V_edges) {
        events_.emplace_back(e.first.y(), START_POINT, &e);
        events_.emplace_back(e.second.y(), END_POINT, &e);
    }
    std::sort(events_.begin(), events_.end(), EventComparator);

    std::set<int> set_;
    for(const auto& event : events_) {
        if(event.type == START_POINT)
            set_.emplace(event.segment->first.x());
        else if(event.type == END_POINT)
            set_.erase(event.segment->first.x());
        else if(event.type == PARALLEL_SEGMENT) {
            int x1 = event.segment->first.x(), x2 = event.segment->second.x();
            auto lower_it = set_.lower_bound(x1 + 1);
            auto upper_it = set_.upper_bound(x2 - 1);
            // +- 1 to prevent equal case
            if((lower_it == upper_it) or (x2 - x1) <= 1) { // no intersection
                H_effective_chords.push_back(*event.segment);
            }
        }
    }
}


void Splitter::_maximum_independent_set(
    const vector<Segment>& H_effective_chords,
    const vector<Segment>& V_effective_chords,
    vector<Segment>& H_selected_chords,
    vector<Segment>& V_selected_chords
    ) {

    vector<Event> events;
    map<Segment const*, int> Hsegment2idx;
    for(int idx = 0; idx<H_effective_chords.size(); idx++) {
        const auto& e = H_effective_chords[idx];
        events.emplace_back(e.first.x(), CHORD_START_POINT, &e);
        events.emplace_back(e.second.x(), CHORD_END_POINT, &e);
        Hsegment2idx[&e] = idx;
    }

    map<Segment const*, int> Vsegment2idx;
    for(int idx = 0; idx < V_effective_chords.size(); idx++) {
        const auto& e = V_effective_chords[idx];
        events.emplace_back(e.first.x(), PARALLEL_SEGMENT, &e);
        Vsegment2idx[&e] = idx;
    }
    std::sort(events.begin(), events.end(), EventComparator);

    multimap<int, Segment const*> map;
    vector<pair<Segment const*, Segment const*>> intersections;
    for(const auto& event : events) {
        if(event.type == CHORD_START_POINT)
            map.emplace(event.segment->first.y(), event.segment);
        else if(event.type == CHORD_END_POINT)
            map.erase(map.find(event.segment->first.y()));
        else if(event.type == PARALLEL_SEGMENT) {
            int y1 = event.segment->first.y(), y2 = event.segment->second.y();
            auto lower_it = map.lower_bound(y1);
            auto upper_it = map.upper_bound(y2);
            // NO +- 1 to prevent equal case

            for(auto it = lower_it; it != upper_it; ++it) {
                intersections.emplace_back(it->second, event.segment);
            }
        }
    }

    BipGraph graph(H_effective_chords.size(), V_effective_chords.size());
    for(const auto& intersection : intersections)
        graph.addEdge(Hsegment2idx[intersection.first] + 1, Vsegment2idx[intersection.second] + 1);

    graph.maximum_independent_set();
    for(auto i: graph.U_ind)
        H_selected_chords.push_back(H_effective_chords[i - 1]);
    for(auto i: graph.V_ind)
        V_selected_chords.push_back(V_effective_chords[i - 1]);
}


vector<bShape *> Splitter::_dissect_by_subregions(bShape* polygon, vector<bSegment> selected_effective_chords) {
    return vector<bShape*> ();
}

vector<bBox *> Splitter::_split_subregion(bShape* subregion, vector<bPoint> concave_vectives) {
    return vector<bBox*> ();
}
