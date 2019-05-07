#ifndef  _DESIGN_H_
#define _DESIGN_H_

#include "parser.h"


class Design {
public:
    void execute(const Operation&);
    void write_output(char*);

private:

    void _merge(const myPolygonList &polygon_list);

    void _clip(const myPolygonList &polygon_list);

    void _split(char type);
};


#endif