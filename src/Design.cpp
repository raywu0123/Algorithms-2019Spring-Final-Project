#include "Design.h"
#include "Operation.h"


void Design::execute(const Operation& op) {
    switch (op.type) {
        case M:
            _merge(op.polygon_list);
            break;
        case C:
            _clip(op.polygon_list);
            break;
        case SV:
        case SH:
        case SO:
            _split(op.type);
            break;
        default: ;
    }
}


void Design::_merge(const myPolygonList& polygon_list) {

}


void Design::_clip(const myPolygonList& polygon_list) {

}

void Design::_split(char type) {

}


void Design::write_output(char *) {}