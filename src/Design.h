#ifndef  _DESIGN_H_
#define _DESIGN_H_

#include <vector>
#include <algorithm>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/polygon/polygon.hpp>
#include <boost/polygon/polygon_90_set_data.hpp>
namespace gtl = boost::polygon;
using namespace boost::polygon::operators;

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef gtl::polygon_data<int> Polygon;

#include "parser.h"
#include "bLib/bLibRTree.h"
#include "bLib/bLibPtr.h"
#include "splitLib/splitter.h"


class Design {
public:
    void execute(const Operation &);
    void write_output(char*);

    vector<vector<int>>  m_mergeIds;  // group some bShape together

private:
    void _split(const string& type);
    void _split_o();
    void _merge(
        const vector<bLib::bShape *>& polygon_list,
        const gtl::orientation_2d& split_orientation = gtl::HORIZONTAL,
        bool verbose = true
    );
    void _clip(const vector<bLib::bShape *>& polygon_list);
    void _maintain_vpoints();
    bool _boxes2vpoints(bLib::bShape* &, vector<bLib::bShape *>&);
    void _maintain_polygon_indexes();
    void _polygon_list_quick_delete(const vector<int>&);
    vector<bLib::bShape*> _polygon_list;

};


#endif
