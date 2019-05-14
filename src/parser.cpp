#include "parser.h"
#include <fstream>

using namespace std;

Parser::Parser() {}


Parser::~Parser() {

}


bool Parser::read_input(char* filename) {
    cout << "STAT| Reading input from " << filename << endl;
    ifstream input_file;
    input_file.open(filename);
    if (!input_file.is_open()) {
        cout << "ERROR| cannot open file:" << filename << endl;
        exit(0);
    }
    _read_header(input_file);
    bool bb = true;
    while (bb) bb = _read_operation(input_file);

    cout << "STAT| Finish reading input from " << filename << endl;
    return true;
}


void Parser::_read_header(ifstream& input_file) {
    string s;
    while(true) {
        input_file >> s;
        if (s == "OPERATION") continue;
        if (s == ";") break;
        operation_name_list.push_back(s);
    }
}


bool Parser::_read_operation(ifstream& input_file) {
    string s, op_type, op_name;
    input_file >> s >> op_type >> op_name;

    Operation* op = new Operation(op_type, op_name);
    _operation_map[op_name] = op;

    getline(input_file, s);
    while(getline(input_file, s)) {
        if (s == "END DATA") break;
        op->add_polygon(s);
    }
    return !input_file.eof();
}

const Operation& Parser::get_operation(int idx) {
    const string& op_name = operation_name_list[idx];
    if (op_name == "SV" or op_name == "SH" or op_name == "SO"){
        Operation* op = new Operation(op_name, op_name);
        _operation_map[op_name] = op;
    }
    return *(_operation_map[op_name]);
}
