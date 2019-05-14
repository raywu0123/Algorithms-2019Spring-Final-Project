#include "bLib/bLibPoint.h"
#include "bLib/bLibPtr.h"
#include "Operation.h"
#include <climits>
#include <sstream>

using namespace bLib;


Operation::~Operation() {
    for(int i=0; i<polygon_list.size(); i++) {
        delete polygon_list[i];
    }
}


void Operation::add_polygon(string description) {
    stringstream ss(description);
    string s;
    int x, y;
    int xl = INT_MAX, yl = INT_MAX;
    int xh = INT_MIN, yh = INT_MIN;
    ss >> s;

    vector<bPoint> vpoints;
    while(ss >> x >> y) {
        vpoints.push_back(bPoint(x, y));
        if (xl > x) xl = x;
        if (yl > y) yl = y;
        if (xh < x) xh = x;
        if (yh < y) yh = y;
    }

    if (vpoints.size() == 0) return;
    vpoints.resize(vpoints.size() - 1, true);

    bShape* pmyshape = new bShape(xl, yl, xh, yh);
    pmyshape->setPoints(vpoints);
    vector<bBox> vBoxes;
    bool bb = PTR::polygon2Rect(vpoints, vBoxes);
    if (bb) pmyshape->setRealBoxes(vBoxes);
    pmyshape->setId(polygon_list.size());
    polygon_list.push_back(pmyshape);
}
