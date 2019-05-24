#include <utility>

#include <utility>

#ifndef MYPOLYGON_OPERATION_H
#define MYPOLYGON_OPERATION_H

#include "bLib/bLibShape.h"
#include "bLib/bLibPoint.h"
#include <vector>

using namespace std;


class Operation {
public:
    Operation(string type, string name): type(std::move(type)), name(std::move(name)) {};
    ~Operation();

    void add_polygon(const string&);

    string type;
    string name;
    vector<bLib::bShape*> polygon_list;
};


#endif //MYPOLYGON_OPERATION_H
