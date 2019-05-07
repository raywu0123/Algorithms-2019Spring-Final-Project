#include "Operation.h"

Operation::Operation() {}

Operation::~Operation() {
    for (int i=0; i<polygon_list.size(); i++) {
        delete polygon_list[i];
    }
}