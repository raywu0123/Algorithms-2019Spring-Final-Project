#include "splitter.h"

namespace gtl = boost::polygon;
typedef gtl::polygon_90_data<int> Polygon;

void Splitter::split(bLib::bShape* polygon) {
    vector<bLib::bPoint> concave_vertices = _get_concave_vertices(polygon);
    vector<bLib::bSegment> effective_chords = _get_effective_chords(concave_vertices);
    vector<bLib::bSegment> selected_effective_chords = _maximum_independent_set(effective_chords);
    vector<bLib::bShape*> subregions = _dissect_by_subregions(polygon, selected_effective_chords);

    vector<bLib::bBox*>& boxes = polygon->m_realBoxes;
    boxes.clear();
    for(int i=0; i<subregions.size(); i++) {
        vector<bLib::bBox*>boxes_in_subregion = _split_subregion(subregions[i], concave_vertices);
        boxes.insert(boxes.end(), boxes_in_subregion.begin(), boxes_in_subregion.end());
    }
}

vector<bLib::bPoint> Splitter::_get_concave_vertices(bLib::bShape *pShape) {
    return vector<bLib::bPoint>();
}

vector<bLib::bSegment> Splitter::_get_effective_chords(vector<bLib::bPoint> concave_vertices) {
    return vector<bLib::bSegment> ();
}

vector<bLib::bSegment> Splitter::_maximum_independent_set(vector<bLib::bSegment> effective_chords) {
    return vector<bLib::bSegment> ();
}

vector<bLib::bShape *> Splitter::_dissect_by_subregions(bLib::bShape* polygon, vector<bLib::bSegment> selected_effective_chords) {
    return vector<bLib::bShape*> ();
}

vector<bLib::bBox *> Splitter::_split_subregion(bLib::bShape* subregion, vector<bLib::bPoint> concave_vectives) {
    return vector<bLib::bBox*> ();
}
