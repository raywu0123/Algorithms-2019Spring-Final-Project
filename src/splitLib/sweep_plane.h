#ifndef MYPOLYGON_SWEEP_PLANE_H
#define MYPOLYGON_SWEEP_PLANE_H

#include "splitter.h"


enum EVENT_TYPES {
    START_POINT,
    PARALLEL_SEGMENT,
    END_POINT,
    UNDEF,
};

typedef struct Event {
    EVENT_TYPES type = UNDEF;
    int pos = 0;
    Segment const * segment = nullptr;

    Event(const int pos, const EVENT_TYPES t, const Segment* s) : type(t), pos(pos), segment(s) {
    }

} Event;


bool EventComparator(Event const& e1, Event const& e2) {
    if(e1.pos != e2.pos) return e1.pos < e2.pos;
    return e1.type < e2.type;
}


#endif //MYPOLYGON_SWEEP_PLANE_H
