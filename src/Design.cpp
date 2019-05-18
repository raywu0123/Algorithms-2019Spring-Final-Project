#include "Design.h"
#include "Operation.h"
#include "tqdm/tqdm.h"

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
    vector<int> sid_to_be_erased;
    tqdm bar;
    for(int i=0; i<m_mergeIds.size(); i++) {
        bar.progress(i, m_mergeIds.size());
        if(m_mergeIds[i].empty()) continue;
        gtl::property_merge_90<int, int> pm;
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

    _polygon_list_quick_delete(sid_to_be_erased);
    _maintain_polygon_indexes();
    std::cout << "STAT| merge complete into " << num << " components." << std::endl;
}


void Design::_maintain_polygon_indexes() {
    for(int i=0; i<_polygon_list.size(); i++) {
        bShape* polygon = _polygon_list[i];
        polygon->setId(i);
    }
}


void Design::_polygon_list_quick_delete(const vector<int>& indexes_to_be_erased) {
    for(int i=0; i<indexes_to_be_erased.size(); i++) {
        delete _polygon_list[indexes_to_be_erased[i]];
        _polygon_list[indexes_to_be_erased[i]] = nullptr;
    }
    _polygon_list.erase(
            std::remove(_polygon_list.begin(), _polygon_list.end(), nullptr),
            _polygon_list.end()
            );
}


void Design::_clip(const vector<bShape*>& new_polygons) {

    // Step 1: init RTree
    bLibRTree<bShape> m_rtree;
    for(int i=0; i < _polygon_list.size(); i++)
        m_rtree.insert(_polygon_list[i]);

    // Step 2: Subtract
    // for all polygons to sub.
    for(int i=0; i < new_polygons.size(); i++) {
        bShape* shape = new_polygons[i];
        int id1 = shape->getId(); // assert(id1 == i);
        m_rtree.search(
            shape->x1(),
            shape->y1(),
            shape->x2(),
            shape->y2());
        int size = bLibRTree<bShape>::s_searchResult.size();
        // for all related polygons.
        for(int j=0; j<size; j++) {
            bShape* adjshape = bLibRTree<bShape>::s_searchResult[j];
            // int id2 = adjshape->getId(); if(id1 == id2) continue;
            
            bool bconnect = false;
            std::vector<bBox*> newBoxes;
            // for all boxes in one related polygon.
            for(int l=0; l<adjshape->m_realBoxes.size(); l++){
                // for all boxes in one polygon to sub.
                for(int k=0; k<shape->m_realBoxes.size(); k++){
                    bBox* box1 = adjshape->m_realBoxes[l];
                    bBox* box2 = shape->m_realBoxes[k];
                    // if nothing to sub, pass the box to newBoxes.
                    if (not box1->overlaps(box2, true)) newBoxes.push_back(box1);
                    // if to sub, pass the subbed result(a vector) to newBoxes.
                    else{
                        std::vector<bBox*> to_append = box1->subtract(box2);
                        if (to_append.size()>0)
                            newBoxes.insert(newBoxes.end(), to_append.begin(), to_append.end());
                    }
                }
            }
            // update the subbed result.
            adjshape->m_realBoxes.clear();
            // delete adjshape->m_realBoxes;
            adjshape->m_realBoxes = newBoxes;
            adjshape->to_update_vpoints = true;
        }
    }
    // _merge(vector<bShape*>());
}

// return true means `curshape` is not broke into piece, we just modify it, so keep it in _polygon_list.
// return false means `curshape` is broke into piece, so discar it in _polygon_list.
bool Design::_boxes2vpoints(bShape* &curshape, vector<bShape *>& result_to_append) {
    assert (curshape->m_realBoxes.size() > 0);
    if(curshape->m_realBoxes.size() == 1){
        bBox *poabox = curshape->m_realBoxes[0];
        curshape->setPoints(vector<bPoint>({bPoint(poabox->x1(), poabox->y1()),
                                            bPoint(poabox->x2(), poabox->y1()),
                                            bPoint(poabox->x2(), poabox->y2()),
                                            bPoint(poabox->x1(), poabox->y2())}));
        return true;
    }
    // Sub-Step 1: init RTree for remained polygon piece of one polygon
    bLibRTree<bBox> box_rtree;
    for (int j = 0; j < curshape->m_realBoxes.size(); j++)
    {
        curshape->m_realBoxes[j]->setId(j);
        box_rtree.insert(curshape->m_realBoxes[j]);
    }
    // Sub-Step 2: build up graph for all boxes.
    Graph G(curshape->m_realBoxes.size());
    for(int i=0; i < curshape->m_realBoxes.size(); i++) {
        bBox* box1 = curshape->m_realBoxes[i];
        int id1 = curshape->m_realBoxes[i]->getId(); assert(id1 == i);
        box_rtree.search(box1);
        int size = bLibRTree<bBox>::s_searchResult.size();
        for(int j=0; j<size; j++) {
            bBox* box2 = bLibRTree<bBox>::s_searchResult[j];
            int id2 = box2->getId(); if(id1 == id2) continue;
            if (box1->overlaps(box2, true)) add_edge(id1, id2, G);
        }
    }
    // Sub-Step 3: ICC for boxes
    vector<int> component(num_vertices(G));
    int num_boxes_group = connected_components(G, &component[0]);
    m_mergeIds.clear(); m_mergeIds.resize(num_boxes_group);
    for(int i=0; i < curshape->m_realBoxes.size(); i++) {
        m_mergeIds[component[i]].push_back(i);
    }
    // Sub-Step 4: merge boxes by boost & update _polygon_list
    // for each group to merge

    for(int i=0; i<m_mergeIds.size(); i++) {
        assert (m_mergeIds[i].size()>0);
        bShape* new_pmyshape;
        if (m_mergeIds[i].size()==1){ // only one box in merged group, vpoints are vertices of the only box.
            bBox *poabox = curshape->m_realBoxes[m_mergeIds[i][0]];
            new_pmyshape = new bShape(poabox->x1(),
                                            poabox->y1(),
                                            poabox->x2(),
                                            poabox->y2());
            new_pmyshape->setRealBoxes(vector<bBox>({*poabox}));
            new_pmyshape->setPoints(vector<bPoint>({bPoint(poabox->x1(), poabox->y1()),
                                                    bPoint(poabox->x2(), poabox->y1()),
                                                    bPoint(poabox->x2(), poabox->y2()),
                                                    bPoint(poabox->x1(), poabox->y2())}));
        } 
        else { // more than one box in merge group, need to find new vpoint.
            gtl::property_merge_90<int, int> pm;
            // for each box in merge group, insert it into pm.
            for(int j=0; j<m_mergeIds[i].size(); j++) {
                int sid = m_mergeIds[i][j];
                bBox* poabox = curshape->m_realBoxes[sid]; // pick out one boxes
                poabox->print();
                pm.insert(
                    gtl::rectangle_data<int>(
                        poabox->x1(),
                        poabox->y1(),
                        poabox->x2(), 
                        poabox->y2()),
                    0);
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
            new_pmyshape = new bShape(xl, yl, xh, yh);
            new_pmyshape->setPoints(vpoints);
            vector<bBox> vBoxes;
            bool bb = PTR::polygon2Rect(vpoints, vBoxes);
            std::cout << "size of " << i << "-th merge group of RealBoxes: " << vBoxes.size() << std::endl;
            for(int jj = 0; jj<vBoxes.size(); jj++) vBoxes[jj].print();
            std::cout << "<=== " << i << "-th merge group\n" << std::endl;
            if (bb) new_pmyshape->setRealBoxes(vBoxes);
        }
        if (m_mergeIds.size() == 1){
            // delete curshape;
            curshape = new_pmyshape;
            return true;
        }
        else{
            result_to_append.push_back(new_pmyshape);
        }
    }
    return false;
}

void Design::_maintain_vpoints(){
    vector<bLib::bShape *> polygon_list_to_append;
    vector<int> sid_to_be_erased;
    for(int idx=0; idx<_polygon_list.size(); idx++) {
        if (_polygon_list[idx]->to_update_vpoints == true){
            if (!_boxes2vpoints(_polygon_list[idx], polygon_list_to_append))
                sid_to_be_erased.push_back(idx);
            _polygon_list[idx]->to_update_vpoints = false;
        }
    }
    for (int i=0; i < polygon_list_to_append.size(); i++)
        _polygon_list.push_back(polygon_list_to_append[i]);
    _polygon_list_quick_delete(sid_to_be_erased);
}

void Design::_split(string type) {
    _maintain_vpoints();
    // TODO
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
