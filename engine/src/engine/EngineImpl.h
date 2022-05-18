//
// Created by Brandon on 5/15/22.
//

#ifndef MACE_ENGINEIMPL_H
#define MACE_ENGINEIMPL_H

#ifdef __ANDROID__
struct android_app;
#endif

#include "../EventSystem.h"

class EngineImpl {
public:
#ifdef __ANDROID__
    void init(android_app* app);
#endif
    void launch();

    EventSystem& getEventSystem();

private:
    void update();
    void draw();
    bool isRunning();

    EventSystem eventSystem;

};


#endif //MACE_ENGINEIMPL_H
