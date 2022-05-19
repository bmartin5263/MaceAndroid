//
// Created by Brandon on 5/16/22.
//

#ifndef MACE_APPRESUMEEVENT_H
#define MACE_APPRESUMEEVENT_H

#include "EventBase.h"

struct AppResumeEvent {
    AppResumeEvent() = default;
    explicit AppResumeEvent(NativeWindow nativeWindow):
        nativeWindow(nativeWindow)
    {}
    NativeWindow nativeWindow;
};

#endif //MACE_APPRESUMEEVENT_H
