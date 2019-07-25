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
    for(auto shape : new_polygons) {
        shape->setId(_polygon_list.size());
        _polygon_list.push_back(shape);
    }
    // Step 1: init RTree
    bLibRTree<bBox> m_rtree;
    for(auto& polygon : _polygon_list) {
        int id = polygon->getId();
        for(auto& rect : polygon->m_realBoxes) {
            rect->setId(id);
            m_rtree.insert(rect);
        }
    }

    // Step 2: build up graph
    Graph G(_polygon_list.size());
    for(auto& polygon : _polygon_list) {
        int id1 = polygon->getId(); // assert(id1 == i);
        for(auto& rect: polygon->m_realBoxes) {
            m_rtree.search(
                rect->x1(),
                rect->y1(),
                rect->x2(),
                rect->y2()
            );
            for(auto& result_rect : bLibRTree<bBox>::s_searchResult) {
                int id2 = result_rect->getId();
                if(id1 != id2 and rect->overlaps(result_rect)) {
                    add_edge(id1, id2, G);
                }
            }
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
        if(m_mergeIds[i].size() <= 1) continue;

        gtl::property_merge_90<int, int> pm;
        for(int sid : m_mergeIds[i]) {
            bShape* pmyshape = _polygon_list[sid];
            assert (!_polygon_list[sid]->m_realBoxes.empty());
            for(auto poabox : pmyshape->m_realBoxes) {
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
        auto* pmyshape = _get_shape_from_merge_result(result);
        _polygon_list.push_back(pmyshape);
    }

    _polygon_list_quick_delete(sid_to_be_erased);
    _maintain_polygon_indexes();
    std::cout << std::endl;
    std::cout << "STAT| Merge complete into " << num << " components" << std::endl;
}


void Design::_maintain_polygon_indexes() {
    for(int i=0; i<_polygon_list.size(); i++) {
        bShape* polygon = _polygon_list[i];
        polygon->setId(i);
    }
}


void Design::_polygon_list_quick_delete(const vector<int>& indexes_to_be_erased) {
    for(int i : indexes_to_be_erased) {
        delete _polygon_list[i];
        _polygon_list[i] = nullptr;
    }
    _polygon_list.erase(
            std::remove(_polygon_list.begin(), _polygon_list.end(), nullptr),
            _polygon_list.end()
            );
}


void Design::_clip(const vector<bShape*>& new_polygons) {

    // Step 1: init RTree
    bLibRTree<bShape> m_rtree;
    for(auto & i : _polygon_list){
        // assert(_polygon_list[i]->m_realBoxes.size() > 0);
        m_rtree.insert(i);
    }

    // Step 2: Subtract
    // for all polygons to sub.
    for(auto& polygon : new_polygons) {
        // int id1 = polygon->getId(); // assert(id1 == i);
        // for all boxes in one polygon to sub.
        for(auto& box2: polygon->m_realBoxes) {
            m_rtree.search(
                box2->x1(),
                box2->y1(),
                box2->x2(),
                box2->y2()
            );
            int size = bLibRTree<bShape>::s_searchResult.size();
            // for all related polygons.
            for(int j=0; j<size; j++) {
                bShape* adjshape = bLibRTree<bShape>::s_searchResult[j];
                
                std::vector<bBox*> newBoxes;
                // for all boxes in one related polygon.
                for(auto box1 : adjshape->m_realBoxes){
                    // if nothing to sub, pass the box to newBoxes.
                    if (not box1->overlaps(box2, false)) newBoxes.push_back(box1);
                    // if to sub, pass the subbed result(a vector) to newBoxes.
                    else{
                        std::vector<bBox*> to_append = box1->subtract(box2);
                        if (!to_append.empty())
                            newBoxes.insert(newBoxes.end(), to_append.begin(), to_append.end());
                        delete box1;
                    }
                }
                // update the subbed result.
                adjshape->m_realBoxes.clear();
                adjshape->m_realBoxes.shrink_to_fit();
                if (!newBoxes.empty())
                    adjshape->m_realBoxes.insert(adjshape->m_realBoxes.end(), newBoxes.begin(), newBoxes.end());
                adjshape->to_update_vpoints = true;
            }
        }
    }
    _maintain_vpoints();
}

// return true means `curshape` is not broke into piece, we just modify it, so keep it in _polygon_list.
// return false means `curshape` is broke into piece, so discard it from _polygon_list.
bool Design::_boxes2vpoints(bShape* &curshape, vector<bShape *>& result_to_append) {
    assert (!curshape->m_realBoxes.empty());
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

    for(auto & m_mergeId : m_mergeIds) {
        assert (!m_mergeId.empty());
        bShape* new_pmyshape;
        if (m_mergeId.size()==1){ // only one box in merged group, vpoints are vertices of the only box.
            bBox *poabox = curshape->m_realBoxes[m_mergeId[0]];
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
            for(int sid : m_mergeId) {
                bBox* poabox = curshape->m_realBoxes[sid]; // pick out one boxes
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
            new_pmyshape = _get_shape_from_merge_result(result);
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


bShape* Design::_get_shape_from_merge_result(
    map< set<int>, gtl::polygon_90_set_data<int> >& result
) {
    set<int> settmp; settmp.insert(0);
    auto itr = result.find(settmp);
    gtl::polygon_90_set_data<int> polyset = itr->second;
    vector<Polygon> output;
    polyset.get_polygons(output);
    Polygon& poly = output[0];

    vector<bPoint> vpoints;
    int xl = INT_MAX, yl = INT_MAX;
    int xh = INT_MIN, yh = INT_MIN;
    auto poly_itr = poly.begin(), poly_end = poly.end();
    for(; poly_itr != poly_end; poly_itr++) {
        int x = gtl::x(*poly_itr);
        int y = gtl::y(*poly_itr);
        vpoints.emplace_back(x, y);
        if (xl > x) xl = x;
        if (yl > y) yl = y;
        if (xh < x) xh = x;
        if (yh < y) yh = y;
    }
    bShape* new_pmyshape = new bShape(xl, yl, xh, yh);
    new_pmyshape->setPoints(vpoints);

    vector<Polygon> rectangles;
    polyset.get_rectangles(rectangles);
    vector<bBox> vBoxes;
    for(auto & rectangle : rectangles) {
        vBoxes.emplace_back(
                rectangle.coords_[0].x(),
                rectangle.coords_[0].y(),
                rectangle.coords_[2].x(),
                rectangle.coords_[2].y()
        );
    }
    new_pmyshape->setRealBoxes(vBoxes);
    return new_pmyshape;
}


void Design::_maintain_vpoints(){
    vector<bLib::bShape *> polygon_list_to_append;
    vector<int> sid_to_be_erased;
    for(int idx=0; idx<_polygon_list.size(); idx++) {
        if (_polygon_list[idx]->to_update_vpoints){
            if (_polygon_list[idx] == nullptr)
                sid_to_be_erased.push_back(idx);
            else if (_polygon_list[idx]->m_realBoxes.empty())
                sid_to_be_erased.push_back(idx);
            else if (!_boxes2vpoints(_polygon_list[idx], polygon_list_to_append))
                sid_to_be_erased.push_back(idx);
            _polygon_list[idx]->to_update_vpoints = false;
        }
    }
    for(auto i : polygon_list_to_append)
        _polygon_list.push_back(i);
    _polygon_list_quick_delete(sid_to_be_erased);
    _maintain_polygon_indexes();
}


void Design::_split(const string& type) {
    for(auto & polygon : _polygon_list) {
        if(type == "SO")    Splitter::split(polygon);
        else if(type == "SV") Splitter::boost_split(polygon, gtl::VERTICAL);
        else if(type == "SH") Splitter::boost_split(polygon, gtl::HORIZONTAL);
        else {
            cerr << "ERR | Incorrect split type: " << type << endl;
            break;
        }
    }
    cout << "STAT| Split complete" << endl;
}


void Design::write_output(char* filename) {
    cout << "STAT| Writing output to " << filename << endl;
    ofstream output_file;
    output_file.open(filename);
    int rect_count = 0;
    for(auto polygon : _polygon_list) {
        for(auto rectangle : polygon->m_realBoxes) {
            output_file << "RECT "
                        << rectangle->x1() << " "
                        << rectangle->y1() << " "
                        << rectangle->x2() << " "
                        << rectangle->y2() << " ;" << endl;
            rect_count ++;
        }
    }
    output_file.close();
    cout << "STAT| Finish writing output to " << filename << endl;
    cout << "STAT| Total of " << rect_count << " rectangles" << endl;
}
