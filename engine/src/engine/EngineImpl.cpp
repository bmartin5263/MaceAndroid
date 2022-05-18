//
// Created by Brandon on 5/15/22.
//

#include "EngineImpl.h"
#include "../EventSystem.h"
#include "../Log.h"

void EngineImpl::launch() {
    update();
    draw();
}

void EngineImpl::update() {
    Log::Error("EngineImpl::update()");
}

void EngineImpl::draw() {
    Log::Error("EngineImpl::draw()");
}

bool EngineImpl::isRunning() {
    return true;
}

EventSystem &EngineImpl::getEventSystem() {
    return eventSystem;
}