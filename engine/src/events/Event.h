//
// Created by Brandon on 5/15/22.
//

#ifndef MACE_EVENT_H
#define MACE_EVENT_H

#include "Events.h"

struct Event {
//    Event(EventType type) :
//        type{type}
//    {}

    EventType type;
    union {
        AppInitEvent appInitEvent;
        AppResumeEvent appResumeEvent;
    };
};


#endif //MACE_EVENT_H
