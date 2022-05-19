//
// Created by Brandon on 5/15/22.
//

#ifndef MACE_ENGINEIMPL_H
#define MACE_ENGINEIMPL_H

#ifdef __ANDROID__
struct android_app;
#endif

#include "graphics/GLContext.h"
#include "Clock.h"
#include "rendering/TexturedTeapotRenderer.h"
#include "rendering/TapCamera.h"
#include "EventSystem.h"

class DeviceEngine;
class EngineImpl {
public:
    explicit EngineImpl(std::unique_ptr<DeviceEngine> deviceEngine);
    void launch();

    EventSystem& getEventSystem();

private:
    void update();
    void draw();
    bool isRunning();

    std::unique_ptr<DeviceEngine> deviceEngine;
    EventSystem eventSystem;

    TapCamera tapCamera;
    GLContext glContext;
    Clock engineClock;

    TexturedTeapotRenderer teapotRender;

};


#endif //MACE_ENGINEIMPL_H
