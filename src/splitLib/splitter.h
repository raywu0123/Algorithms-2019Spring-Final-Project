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
typedef pair<Point, Point> Segment;

typedef gtl::polygon_90_set_data<int> PolygonSet;
typedef gtl::rectangle_data<int> Rectangle;

class Splitter {
public:
    void split(bLib::bShape*);

private:

    HolePolygon _build_graph(bShape*);
    vector<vector<Point>> _get_loops(const HolePolygon&);
    void _get_edges(const vector<vector<Point>>&, vector<Segment>&, vector<Segment>&);
    vector<pair<Point, Point>> _get_concave_vertices(const vector<vector<Point>>&);
    void _get_effective_chords(
        vector<pair<Point, Point>>&,
        const vector<Segment>&, const vector<Segment>&,
        vector<Segment>&, vector<Segment>&
    );
    void _maximum_independent_set(
        const vector<Segment>&, const vector<Segment>&,
        vector<Segment>&, vector<Segment>&
    );
    vector<vector<Point>> _dissect_by_subregions(
        const vector<vector<Point>>&,
        const vector<Segment>&,
        const vector<Segment>&
    );
    vector<bBox> _split_subregion(const vector<Point>&);
};


#endif //MYPOLYGON_SPLITTER_H
