#include "parser.h"
#include "Design.h"
#include <iostream>

using namespace std;


void print_usage() {
    cout << endl;
    cout << "==================   Command-Line Usage   ====================" << endl;
    cout << "./myPolygon <input_file> <output_file>" << endl;
    cout << endl;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    Parser* parser = new Parser();
    parser->read_input(argv[0]);

    Design* design = new Design();
    for (int i=0; i < parser->size(); i++) {
        design->execute(parser->get_operation(i));
    }

    design->write_output(argv[1]);
}
