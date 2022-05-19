//
// Created by Brandon on 5/13/22.
//

#include "MyEngine.h"
#include "mace/engine/EngineImpl.h"
#include "mace/engine/DeviceEngine.h"

DeviceEngine* MyEngine::deviceEngine;

EngineImpl& MyEngine::Instance() {
    static EngineImpl engine{std::unique_ptr<DeviceEngine>(deviceEngine)};   // the singleton from which all other Engine systems belong to
    return engine;
}

void MyEngine::Launch() {
    Instance().launch();
}

EventSystem& MyEngine::EventSystem() {
    return Instance().getEventSystem();
}
