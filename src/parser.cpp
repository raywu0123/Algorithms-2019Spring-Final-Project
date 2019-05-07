#include "parser.h"


Parser::Parser() {}


Parser::~Parser() {
    for (int i=0; i<operation_list.size(); i++) {
        delete operation_list[i];
    }
}


bool Parser::read_input(char* filename) {
    return true;
}


const Operation& Parser::get_operation(int idx) {
    return *(operation_list[idx]);
}