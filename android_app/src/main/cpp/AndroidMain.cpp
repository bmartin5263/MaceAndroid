//
// Created by Brandon on 5/15/22.
//

#include <android_native_app_glue.h>
#include <android/log.h>
#include "OriginalEngine.h"
#include "CoreEngine.h"
#include "Game.h"

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* app) {
    Game g{};
    g.start();

    LOGI("android_main()");
    mace::CoreEngine::Init(app);
    mace::CoreEngine::Launch();

    OriginalEngine engine{app};
    engine.run();
}
