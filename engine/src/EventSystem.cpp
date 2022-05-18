//
// Created by Brandon on 5/15/22.
//

#include "EventSystem.h"
#include "MyEngine.h"

EventSystem &EventSystem::Instance() {
    return MyEngine::EventSystem();
}

void EventSystem::pushEvent(const Event &event) {
    eventQueue.push(event);
}

void EventSystem::PushEvent(const Event &event) {
    Instance().pushEvent(event);
}
