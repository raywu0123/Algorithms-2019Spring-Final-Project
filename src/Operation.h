#ifndef MYPOLYGON_OPERATION_H
#define MYPOLYGON_OPERATION_H

#include <vector>

using namespace std;

typedef vector<pair<int, int>> myPolygon;
typedef vector<myPolygon*> myPolygonList;


enum operation_type {
    M,
    C,
    SV,
    SH,
    SO,
};


class Operation {
public:
    Operation();
    ~Operation();

    char type;
    myPolygonList polygon_list;
};


#endif //MYPOLYGON_OPERATION_H
