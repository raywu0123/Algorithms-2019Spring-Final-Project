#ifndef  _DESIGN_H_
#define _DESIGN_H_

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/polygon/polygon.hpp>
namespace gtl = boost::polygon;
using namespace boost::polygon::operators;

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef gtl::polygon_data<int> Polygon;
typedef gtl::rectangle_data<int> Rectangle;
typedef gtl::point_data<int> Point;

#include "parser.h"
#include "bLib/bLibRTree.h"
#include "bLib/bLibPtr.h"


class Design {
public:
    void execute(const Operation&);
    void write_output(char*);

    vector<vector<int>>  m_mergeIds;  // group some bShape together

// private:
    void _split(string type);
    void _merge(const vector<bLib::bShape *>& polygon_list);
    void _clip(const vector<bLib::bShape *>& polygon_list);
    void _maintain_polygon_indexes();
    void _polygon_list_quick_delete(int);

    vector<bLib::bShape*> _polygon_list;

};


#endif
