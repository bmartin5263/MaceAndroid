//
// Created by Brandon on 5/13/22.
//

#include "MyEngine.h"
#include "engine/EngineImpl.h"

EngineImpl& MyEngine::Instance() {
    static EngineImpl engine;   // the singleton from which all other Engine systems belong to
    return engine;
}

void MyEngine::Launch() {
    Instance().launch();
}

EventSystem& MyEngine::EventSystem() {
    return Instance().getEventSystem();
}
