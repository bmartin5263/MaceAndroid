//
// Created by Brandon on 5/15/22.
//

#ifndef MACE_EVENTSYSTEM_H
#define MACE_EVENTSYSTEM_H

#include <queue>
#include "events/Event.h"


class EventSystem {
public:
    static void PushEvent(const Event& event) { instance().pushEvent(event); };
    void pushEvent(const Event& event);

private:
    static EventSystem& instance();

    std::queue<Event> eventQueue;
};


#endif //MACE_EVENTSYSTEM_H
