//
// Created by Brandon on 5/15/22.
//

#include "EngineImpl.h"
#include "../EventSystem.h"

void EngineImpl::launch() {
    update();
    draw();
}

void EngineImpl::update() {

}

void EngineImpl::draw() {

}

bool EngineImpl::isRunning() {
    return true;
}

EventSystem &EngineImpl::getEventSystem() {
    return eventSystem;
}
