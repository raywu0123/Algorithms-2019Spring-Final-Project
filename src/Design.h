#ifndef  _DESIGN_H_
#define _DESIGN_H_

#include "parser.h"


class Design {
public:
    void execute(const Operation&);
    void write_output(char*);

private:
    void _split(string type);
    void _merge(vector<bLib::bShape *> polygon_list);
    void _clip(vector<bLib::bShape *> polygon_list);
};


#endif