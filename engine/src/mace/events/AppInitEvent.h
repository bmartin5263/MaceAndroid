//
// Created by Brandon on 5/16/22.
//

#ifndef MACE_APPINITEVENT_H
#define MACE_APPINITEVENT_H

#include "EventBase.h"

struct AppInitEvent {
    AppInitEvent() = default;
    explicit AppInitEvent(NativeWindow nativeWindow):
        nativeWindow(nativeWindow)
    {}

    NativeWindow nativeWindow;
};

#endif //MACE_APPINITEVENT_H
