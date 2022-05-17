//
// Created by Brandon on 5/15/22.
//

#include "EventSystem.h"
#include "MyEngine.h"

EventSystem &EventSystem::instance() {
    return MyEngine::EventSystem();
}

void EventSystem::pushEvent(const Event &event) {
    eventQueue.push(event);
}
