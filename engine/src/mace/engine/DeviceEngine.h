//
// Created by Brandon on 5/18/22.
//

#ifndef MACE_DEVICEENGINE_H
#define MACE_DEVICEENGINE_H

#ifdef __ANDROID__
#include <android_native_app_glue.h>
#include "../../../../android_engine/src/cpp/mace/ndk/NDKHelper.h"
#endif

class DeviceEngine {
public:
    void launch(EngineImpl* engine);

#ifdef __ANDROID__
public:
    DeviceEngine(android_app* androidApp);
private:
    android_app* app;
    ndk_helper::DoubletapDetector doubletapDetector;
    ndk_helper::DragDetector dragDetector;
    ndk_helper::PinchDetector pinchDetector;
    ndk_helper::SensorManager sensorManager;
#endif


};


#endif //MACE_DEVICEENGINE_H
