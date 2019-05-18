#ifndef MYPOLYGON_SPLITTER_H
#define MYPOLYGON_SPLITTER_H

#include <boost/polygon/polygon.hpp>
#include "../bLib/bLibShape.h"
#include <vector>
#include <iostream>

using namespace std;


class Splitter {
public:
    void split(bLib::bShape*);

private:

    vector<bLib::bPoint> _get_concave_vertices(bLib::bShape *pShape);

    vector<bLib::bSegment> _get_effective_chords(vector<bLib::bPoint> vector);

    vector<bLib::bSegment> _maximum_independent_set(vector<bLib::bSegment> vector);

    vector<bLib::bShape *> _dissect_by_subregions(bLib::bShape *pShape, vector<bLib::bSegment> vector);

    vector<bLib::bBox *> _split_subregion(bLib::bShape* pShape, vector<bLib::bPoint> vector);
};




#endif //MYPOLYGON_SPLITTER_H
