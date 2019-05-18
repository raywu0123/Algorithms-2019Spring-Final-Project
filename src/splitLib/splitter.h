#ifndef MYPOLYGON_SPLITTER_H
#define MYPOLYGON_SPLITTER_H

#include <boost/polygon/polygon.hpp>
#include "../bLib/bLibShape.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace bLib;

namespace gtl = boost::polygon;

typedef gtl::polygon_data<int> Polygon;
typedef gtl::polygon_90_with_holes_data<int> HolePolygon;
typedef gtl::point_data<int> Point;

typedef gtl::polygon_90_set_data<int> PolygonSet;
typedef gtl::rectangle_data<int> Rectangle;

class Splitter {
public:
    void split(bLib::bShape*);

private:

    HolePolygon _build_graph(bShape*);
    vector<Point> _get_concave_vertices(const HolePolygon&);
    vector<bSegment> _get_effective_chords(vector<bPoint> vector);
    vector<bSegment> _maximum_independent_set(vector<bSegment> vector);
    vector<bShape *> _dissect_by_subregions(bShape *pShape, vector<bSegment> vector);
    vector<bBox *> _split_subregion(bShape* pShape, vector<bPoint> vector);
};




#endif //MYPOLYGON_SPLITTER_H
