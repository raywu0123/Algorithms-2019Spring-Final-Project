#include "splitter.h"

namespace gtl = boost::polygon;


void Splitter::split(bShape* polygon) {
    HolePolygon hp = _build_graph(polygon);

    const vector<Point>& concave_vertices = _get_concave_vertices(hp);
//    vector<bSegment> effective_chords = _get_effective_chords(concave_vertices);
//    vector<bSegment> selected_effective_chords = _maximum_independent_set(effective_chords);
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

vector<Point> Splitter::_get_concave_vertices(const HolePolygon& hp) {
    vector<vector<Point>> loops;
    vector<Point> exterior;
    for(auto it=hp.begin(); it != hp.end(); ++it)
        exterior.push_back(*it);
    exterior.insert(exterior.end(), hp.begin(), std::next(hp.begin(), 2));
    if(gtl::winding(hp) != gtl::CLOCKWISE) std::reverse(exterior.begin(), exterior.end());
    loops.push_back(exterior);

    for(auto it=hp.begin_holes(); it != hp.end_holes(); ++it) {
        const auto& hole_polygon = *it;
        vector<Point> hole;
        for(auto pit=hole_polygon.begin(); pit != hole_polygon.end(); ++pit)
            hole.push_back(*pit);
        hole.insert(hole.end(), hole_polygon.begin(), std::next(hole_polygon.begin(), 2));
        if(gtl::winding(hole_polygon) != gtl::COUNTERCLOCKWISE) std::reverse(hole.begin(), hole.end());
        loops.push_back(hole);
    }

    vector<Point> concave_vertices;
    for(auto loop : loops) {
        for(int vertice_idx=1; vertice_idx < loop.size() - 1; vertice_idx++) {
            int dx1 = loop[vertice_idx].x() - loop[vertice_idx - 1].x();
            int dy1 = loop[vertice_idx].y() - loop[vertice_idx - 1].y();
            int dx2 = loop[vertice_idx + 1].x() - loop[vertice_idx].x();
            int dy2 = loop[vertice_idx + 1].y() - loop[vertice_idx].y();
            if(dx1 * dy2 - dx2 * dy1 > 0) // determine concavity by outer product of edge vectors
                concave_vertices.push_back(loop[vertice_idx]);
        }
    }
    return concave_vertices;
}


vector<bSegment> Splitter::_get_effective_chords(vector<bPoint> concave_vertices) {
    return vector<bSegment> ();
}

vector<bSegment> Splitter::_maximum_independent_set(vector<bSegment> effective_chords) {
    return vector<bSegment> ();
}

vector<bShape *> Splitter::_dissect_by_subregions(bShape* polygon, vector<bSegment> selected_effective_chords) {
    return vector<bShape*> ();
}

vector<bBox *> Splitter::_split_subregion(bShape* subregion, vector<bPoint> concave_vectives) {
    return vector<bBox*> ();
}
