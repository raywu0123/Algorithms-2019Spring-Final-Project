#ifndef  _PARSER_H_
#define _PARSER_H_

#include "Operation.h"
#include <vector>
#include <map>

using namespace std;


typedef vector<string> OperationNameList;
typedef map<string, Operation*> OperationMap;

class Parser {
public:
    Parser();
    ~Parser();
    bool read_input(char*);
    int size() {return operation_name_list.size();};
    const Operation& get_operation(int);

    OperationNameList operation_name_list;
private:
    void _read_header(ifstream& input_file);
    bool _read_operation(ifstream &input_file);

    OperationMap _operation_map;

};


#endif