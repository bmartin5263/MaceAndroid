//
// Created by Brandon on 5/15/22.
//

#include <android_native_app_glue.h>
#include <android/log.h>
#include "Engine.h"
#include "MyEngine.h"

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* app) {
    LOGI("android_main()");
    MyEngine::Init(app);
    MyEngine::Launch();

    Engine engine{app};
    engine.run();
}
