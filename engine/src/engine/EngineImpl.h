//
// Created by Brandon on 5/15/22.
//

#ifndef MACE_ENGINEIMPL_H
#define MACE_ENGINEIMPL_H

#include "../EventSystem.h"

class EngineImpl {
public:
    void launch();

    EventSystem& getEventSystem();

private:
    void update();
    void draw();
    bool isRunning();

    EventSystem eventSystem;

};


#endif //MACE_ENGINEIMPL_H
