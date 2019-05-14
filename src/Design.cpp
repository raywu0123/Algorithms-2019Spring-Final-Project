#include "Design.h"
#include "Operation.h"

using namespace std;
using namespace bLib;

void Design::execute(const Operation& op) {
    if(op.type == "MERGE") _merge(op.polygon_list);
    else if (op.type == "CLIPPER") _clip(op.polygon_list);
    else _split(op.type);
}


void Design::_merge(const vector<bShape*>& new_polygons) {
    for(int i=0; i<new_polygons.size(); i++) {
        bShape* shape = new_polygons[i];
        shape->setId(_polygon_list.size());
        _polygon_list.push_back(shape);
    }

    // Step 1: init RTree
    bLibRTree<bShape> m_rtree;
    for(int i=0; i < _polygon_list.size(); i++)
        m_rtree.insert(_polygon_list[i]);

    // Step 2: build up graph
    Graph G(_polygon_list.size());
    for(int i=0; i < _polygon_list.size(); i++) {
        bShape* shape = _polygon_list[i];
        int id1 = shape->getId(); // assert(id1 == i);
        m_rtree.search(
            shape->x1(),
            shape->y1(),
            shape->x2(),
            shape->y2());
        int size = bLibRTree<bShape>::s_searchResult.size();
        for(int j=0; j<size; j++) {
            bShape* adjshape = bLibRTree<bShape>::s_searchResult[j];
            int id2 = adjshape->getId(); if(id1 == id2) continue;
            
            bool bconnect = false;
            for(int k=0; k<_polygon_list[id1]->m_realBoxes.size(); k++)
            for(int l=0; l<_polygon_list[id2]->m_realBoxes.size(); l++) {
                bBox* box1 = _polygon_list[id1]->m_realBoxes[k];
                bBox* box2 = _polygon_list[id2]->m_realBoxes[l];
                if (not box1->overlaps(box2, true)) continue;
                bconnect = true;
            }

            if(bconnect) add_edge(id1, id2, G);
        }
    }

    // Step 3: ICC
    vector<int> component(num_vertices(G));
    int num = connected_components(G, &component[0]);
    m_mergeIds.clear(); m_mergeIds.resize(num);
    for(int i=0; i < _polygon_list.size(); i++) {
        m_mergeIds[component[i]].push_back(i);
    }

    // Step 4: merge by boost & update _polygon_list
    gtl::property_merge_90<int, int> pm;
    vector<int> sid_to_be_erased;
    for(int i=0; i<m_mergeIds.size(); i++) {
        for(int j=0; j<m_mergeIds[i].size(); j++) {
            int sid = m_mergeIds[i][j];
            bShape* pmyshape = _polygon_list[sid];
            for(int k=0; k<pmyshape->m_realBoxes.size(); k++) {
                bBox* poabox = pmyshape->m_realBoxes[k];
                pm.insert(
                    gtl::rectangle_data<int>(
                        poabox->x1(),
                        poabox->y1(),
                        poabox->x2(),
                        poabox->y2()),
                    0
                );
            }
            sid_to_be_erased.push_back(sid);
        }
        map< set<int>, gtl::polygon_90_set_data<int> > result;
        pm.merge(result);
        set<int> settmp; settmp.insert(0);
        map< set<int>, gtl::polygon_90_set_data<int> >::iterator itr = result.find(settmp);
        gtl::polygon_90_set_data<int> polyset = itr->second;
        vector<Polygon> output;
        polyset.get_polygons(output);
        Polygon& poly = output[0];

        vector<bPoint> vpoints;
        int xl = INT_MAX, yl = INT_MAX;
        int xh = INT_MIN, yh = INT_MIN;
        Polygon::iterator_type poly_itr = poly.begin(), poly_end = poly.end();
        for(; poly_itr != poly_end; poly_itr++) {
            int x = gtl::x(*poly_itr);
            int y = gtl::y(*poly_itr);
            vpoints.push_back(bPoint(x, y));
            if (xl > x) xl = x;
            if (yl > y) yl = y;
            if (xh < x) xh = x;
            if (yh < y) yh = y;
        }
        bShape* pmyshape = new bShape(xl, yl, xh, yh);
        pmyshape->setPoints(vpoints);
        vector<bBox> vBoxes;
        bool bb = PTR::polygon2Rect(vpoints, vBoxes);
        if (bb) pmyshape->setRealBoxes(vBoxes);
        _polygon_list.push_back(pmyshape);
    }

    for(int i=0; i<sid_to_be_erased.size(); i++) {
        _polygon_list_quick_delete(sid_to_be_erased[i]);
    }
    _maintain_polygon_indexes();
}


void Design::_maintain_polygon_indexes() {
    for(int i=0; i<_polygon_list.size(); i++) {
        bShape* polygon = _polygon_list[i];
        polygon->setId(i);
    }
}


void Design::_polygon_list_quick_delete(int idx) {
    if (idx >= _polygon_list.size())
        return;
    _polygon_list[idx] = _polygon_list.back();
    _polygon_list.pop_back();
}


void Design::_clip(const vector<bShape*>& polygon_list) {

}

void Design::_split(string type) {

}


void Design::write_output(char* filename) {
    cout << "STAT| Writing output to " << filename << endl;
    ofstream output_file;
    output_file.open(filename);
    for(int i=0; i<_polygon_list.size(); i++) {
        bShape* polygon = _polygon_list[i];
        for(int i=0; i<polygon->m_realBoxes.size(); i++) {
            bBox* rectangle = polygon->m_realBoxes[i];
            output_file << "RECT "
                        << rectangle->x1() << " "
                        << rectangle->y1() << " "
                        << rectangle->x2() << " "
                        << rectangle->y2() << " ;" << endl;
        }
    }
    output_file.close();
    cout << "STAT| Finish writing output to " << filename << endl;
}
