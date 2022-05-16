/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Engine.h"
#include "MyEngine.h"

Engine::Engine(android_app* app):
    initialized(false),
    focused(false),
    app(app)
{
    doubletapDetector.setConfiguration(this->app->config);
    dragDetector.setConfiguration(this->app->config);
    pinchDetector.setConfiguration(this->app->config);
    teapotRender.bind(&tapCamera);
    LOGI("Engine Initialized");
}

void Engine::loadResources() {
    teapotRender.init(app->activity->assetManager);
}

void Engine::unloadResources() {
    teapotRender.unload();
}

/**
 * Initialize an EGL context for the current display.
 */
int Engine::initWindow(android_app* app) {
    if (!initialized) {
        LOGI("Engine::initWindow initializing resources");
        glContext.init(app->window);
        loadResources();
        initialized = true;
    }
    else if (app->window != glContext.getANativeWindow()) {
        // Re-initialize ANativeWindow.
        // On some devices, ANativeWindow is re-created when the app is resumed
        LOGI("Engine::initWindow reinitializing resources");
        assert(glContext.getANativeWindow());
        unloadResources();
        glContext.invalidate();
        this->app = app;
        glContext.init(app->window);
        loadResources();
        initialized = true;
    } else {
        // initialize OpenGL ES and EGL
        LOGI("Engine::initWindow resuming");
        if (EGL_SUCCESS == glContext.resume(app->window)) {
            unloadResources();
            loadResources();
        } else {
            assert(false);
        }
    }

    showUI();

    // Initialize GL state.
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Note that screen size might have been changed
    glViewport(0, 0, glContext.getScreenWidth(),
               glContext.getScreenHeight());
    teapotRender.updateViewport();

    tapCamera.setFlip(1.f, -1.f, -1.f);
    tapCamera.setPinchTransformFactor(2.f, 2.f, 8.f);

    return 0;
}

void Engine::updateFPS() {
    float fps;
    if (engineClock.update(fps)) {
        JNIEnv *jni;
        app->activity->vm->AttachCurrentThread(&jni, nullptr);

        // Default class retrieval
        jclass clazz = jni->GetObjectClass(app->activity->clazz);
        jmethodID methodID = jni->GetMethodID(clazz, "updateFPS", "(F)V");
        jni->CallVoidMethod(app->activity->clazz, methodID, fps);

        app->activity->vm->DetachCurrentThread();
    }
}

void Engine::drawFrame() {
    updateFPS();
    teapotRender.update(static_cast<float>(Clock::GetCurrentTime()));

    // Just fill the screen with a color.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    teapotRender.render();

    // Swap
    if (EGL_SUCCESS != glContext.swap()) {
        LOGW("Engine::drawFrame() - swap failed");
        unloadResources();
        loadResources();
    }
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void Engine::termWindow() {
    glContext.suspend();
}

void Engine::trimMemory() {
    LOGE("Trimming memory");
    glContext.invalidate();
}

void Engine::handleDrag(AInputEvent* event) {
    using namespace ndk_helper;
    GestureState dragState = dragDetector.detect(event);
    if (dragState & GESTURE_STATE_START) {
        // Otherwise, start dragging
        Vec2 v;
        dragDetector.getPointer(v);
        transformPosition(v);
        tapCamera.beginDrag(v);
    } else if (dragState & GESTURE_STATE_MOVE) {
        Vec2 v;
        dragDetector.getPointer(v);
        transformPosition(v);
        tapCamera.drag(v);
    } else if (dragState & GESTURE_STATE_END) {
        tapCamera.endDrag();
    }
}

void Engine::handlePinch(AInputEvent* event) {
    using namespace ndk_helper;
    GestureState pinchState = pinchDetector.detect(event);
    if (pinchState & ndk_helper::GESTURE_STATE_START) {
        // Start new pinch
        Vec2 v1, v2;
        pinchDetector.getPointers(v1, v2);
        transformPosition(v1,v2);
        tapCamera.beginPinch(v1, v2);
    } else if (pinchState & ndk_helper::GESTURE_STATE_MOVE) {
        // Multi touch
        // Start new pinch
        Vec2 v1, v2;
        pinchDetector.getPointers(v1, v2);
        transformPosition(v1, v2);
        tapCamera.pinch(v1, v2);
    }
}

/**
 * Process the next input event.
 */
int32_t Engine::OnInputEvent(android_app* app, AInputEvent* event) {
    auto* engine = static_cast<Engine*>(app->userData);
    return engine->handleInputEvent(app, event);
}

/**
 * Process the next main command.
 */
void Engine::OnAppCmd(android_app* app, int32_t cmd) {
    auto* engine = static_cast<Engine*>(app->userData);
    engine->handleCmd(app, cmd);
}

bool Engine::hasFocus() const {
    return focused;
}

void Engine::transformPosition(Vec2 &vec) {
    auto w = static_cast<float>(glContext.getScreenWidth());
    auto h = static_cast<float>(glContext.getScreenHeight());
    vec = Vec2(2.0f, 2.0f) * vec /
          Vec2(w,h) -
          Vec2(1.f, 1.f);
}

void Engine::transformPosition(Vec2 &v1, Vec2 &v2) {
    transformPosition(v1);
    transformPosition(v2);
}

void Engine::showUI() {
    LOGI("Engine::showUI()");
    JNIEnv *jni;
    app->activity->vm->AttachCurrentThread(&jni, nullptr);

    // Default class retrieval
    jclass clazz = jni->GetObjectClass(app->activity->clazz);
    jmethodID methodID = jni->GetMethodID(clazz, "showUI", "()V");
    jni->CallVoidMethod(app->activity->clazz, methodID);

    app->activity->vm->DetachCurrentThread();
}

void Engine::run() {
    ndk_helper::JNIHelper::Init(app->activity, HELPER_CLASS_NAME);

    app->userData = this;
    app->onAppCmd = Engine::OnAppCmd;
    app->onInputEvent = Engine::OnInputEvent;
    sensorManager.init(app);

#ifdef USE_NDK_PROFILER
    monstartup("libSpacegameActivity.so");
#endif

    while (update()) {
        draw();
    }
}

bool Engine::update() {
    int id;
    int events;
    android_poll_source *source;
    while ((id = ALooper_pollAll(
            hasFocus() ? 0 : -1,
            nullptr,
            &events,
            (void **) &source
    )) >= 0)
    {
        // Process this event.
        if (source != nullptr) {
            source->process(app, source);
        }

        sensorManager.process(id);

        // Check if we are exiting.
        if (app->destroyRequested != 0) {
            termWindow();
            return false;
        }
    }
    return true;
}

void Engine::draw() {
    if (hasFocus()) {
        // Drawing is throttled to the screen update rate, so there
        // is no need to do timing here.
        drawFrame();
    }
}

void Engine::handleCmd(android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_START:
            LOGI("Processing APP_CMD_START");
            break;
        case APP_CMD_RESUME:
            LOGI("Processing APP_CMD_RESUME");
            break;
        case APP_CMD_INPUT_CHANGED:
            LOGI("Processing APP_CMD_INPUT_CHANGED");
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            LOGI("Processing APP_CMD_INIT_WINDOW");
            if (app->window != nullptr) {
                initWindow(app);
                focused = true;
                drawFrame();
            }
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOGI("Processing APP_CMD_WINDOW_RESIZED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOGI("Processing APP_CMD_CONTENT_RECT_CHANGED");
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            LOGI("Processing APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("Processing APP_CMD_GAINED_FOCUS");
            sensorManager.resume();
            // Start animation
            focused = true;
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("Processing APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_LOW_MEMORY:
            // Free up GL resources
            LOGI("Processing APP_CMD_LOW_MEMORY");
            trimMemory();
            break;
        // Following cases executed in order when navigating away
        case APP_CMD_PAUSE:
            LOGI("Processing APP_CMD_PAUSE");
            break;

        case APP_CMD_LOST_FOCUS:
            // Can occur when looking at all apps scroll view
            LOGI("Processing APP_CMD_LOST_FOCUS");
            sensorManager.suspend();
            // Also stop animating.
            focused = false;
            drawFrame();
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            LOGI("Processing APP_CMD_TERM_WINDOW");
            termWindow();
            focused = false;
            break;
        case APP_CMD_STOP:
            LOGI("Processing APP_CMD_STOP");
            break;
        case APP_CMD_SAVE_STATE:
            LOGI("Processing APP_CMD_SAVE_STATE");
            break;
        case APP_CMD_DESTROY:
            LOGI("Processing APP_CMD_DESTROY");
            break;
        default:
            LOGW("Unhandled Command %d", cmd);
    }
}

int32_t Engine::handleInputEvent(android_app *app, AInputEvent *event) {
    using namespace ndk_helper;
    auto* engine = static_cast<Engine*>(app->userData);
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        // Double tap detector has a priority over other detectors
        GestureState doubleTapState = engine->doubletapDetector.detect(event);
        if (doubleTapState == GESTURE_STATE_ACTION) {
            // Detect double tap
            engine->tapCamera.reset(true);
        } else {
            engine->handleDrag(event);
            engine->handlePinch(event);
        }
        return 1;
    }
    return 0;
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* app) {
    LOGI("android_main()");
    MyEngine myEngine{app};
    Engine engine{app};
    engine.run();
}
