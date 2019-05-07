#ifndef  _PARSER_H_
#define _PARSER_H_

#include "Operation.h"
#include <vector>

using namespace std;


typedef vector<Operation*> myOperationList;

class Parser {
public:
    Parser();
    ~Parser();
    bool read_input(char*);
    int size() {return operation_list.size();};
    const Operation& get_operation(int);

private:
    myOperationList operation_list;
};


#endif