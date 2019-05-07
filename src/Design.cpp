#include "Design.h"
#include "Operation.h"


void Design::execute(const Operation& op) {
    if(op.type == "MERGE") _merge(op.polygon_list);
    else if (op.type == "CLIPPER") _clip(op.polygon_list);
    else _split(op.type);
}


void Design::_merge(vector<bLib::bShape*> polygon_list) {

}


void Design::_clip(vector<bLib::bShape*> polygon_list) {

}

void Design::_split(string type) {

}


void Design::write_output(char *) {}