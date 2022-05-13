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

Engine::Engine(android_app *app):
    initialized_resources_(false),
    has_focus_(false),
    app(app),
    sensor_manager_(nullptr),
    accelerometer_sensor_(nullptr),
    sensor_event_queue_(nullptr)
{
    gl_context_ = ndk_helper::GLContext::GetInstance();
    doubletap_detector_.SetConfiguration(this->app->config);
    drag_detector_.SetConfiguration(this->app->config);
    pinch_detector_.SetConfiguration(this->app->config);
    LOGI("Engine Initialized");
}

void Engine::loadResources() {
    renderer_.Init(app->activity->assetManager);
    renderer_.Bind(&tap_camera_);
}

void Engine::unloadResources() {
    renderer_.Unload();
}

/**
 * Initialize an EGL context for the current display.
 */
int Engine::initWindow(android_app *app) {
    if (!initialized_resources_) {
        LOGI("Engine::initWindow initializing resources");
        gl_context_->init(app->window);
        loadResources();
        initialized_resources_ = true;
    }
    else if (app->window != gl_context_->getANativeWindow()) {
        // Re-initialize ANativeWindow.
        // On some devices, ANativeWindow is re-created when the app is resumed
        LOGI("Engine::initWindow reinitializing resources");
        assert(gl_context_->getANativeWindow());
        unloadResources();
        gl_context_->invalidate();
        this->app = app;
        gl_context_->init(app->window);
        loadResources();
        initialized_resources_ = true;
    } else {
        // initialize OpenGL ES and EGL
        LOGI("Engine::initWindow resuming");
        if (EGL_SUCCESS == gl_context_->resume(app->window)) {
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
    glViewport(0, 0, gl_context_->getScreenWidth(),
               gl_context_->getScreenHeight());
    renderer_.UpdateViewport();

    tap_camera_.SetFlip(1.f, -1.f, -1.f);
    tap_camera_.SetPinchTransformFactor(2.f, 2.f, 8.f);

    return 0;
}

void Engine::updateFPS() {
    float fps;
    if (monitor_.Update(fps)) {
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
    renderer_.Update(static_cast<float>(ndk_helper::PerfMonitor::GetCurrentTime()));

    // Just fill the screen with a color.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer_.Render();

    // Swap
    if (EGL_SUCCESS != gl_context_->swap()) {
        LOGW("Engine::drawFrame() - swap failed");
        unloadResources();
        loadResources();
    }
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void Engine::termWindow() {
    gl_context_->suspend();
}

void Engine::trimMemory() {
    LOGE("Trimming memory");
    gl_context_->invalidate();
}

/**
 * Process the next input event.
 */
int32_t Engine::onInputEvent(android_app *app, AInputEvent *event) {
    auto *eng = static_cast<Engine*>(app->userData);
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        ndk_helper::GESTURE_STATE doubleTapState =
                eng->doubletap_detector_.Detect(event);
        ndk_helper::GESTURE_STATE dragState = eng->drag_detector_.Detect(event);
        ndk_helper::GESTURE_STATE pinchState = eng->pinch_detector_.Detect(event);

        // Double tap detector has a priority over other detectors
        if (doubleTapState == ndk_helper::GESTURE_STATE_ACTION) {
            // Detect double tap
            eng->tap_camera_.Reset(true);
        } else {
            // Handle drag state
            if (dragState & ndk_helper::GESTURE_STATE_START) {
                // Otherwise, start dragging
                ndk_helper::Vec2 v;
                eng->drag_detector_.GetPointer(v);
                eng->TransformPosition(v);
                eng->tap_camera_.BeginDrag(v);
            } else if (dragState & ndk_helper::GESTURE_STATE_MOVE) {
                ndk_helper::Vec2 v;
                eng->drag_detector_.GetPointer(v);
                eng->TransformPosition(v);
                eng->tap_camera_.Drag(v);
            } else if (dragState & ndk_helper::GESTURE_STATE_END) {
                eng->tap_camera_.EndDrag();
            }

            // Handle pinch state
            if (pinchState & ndk_helper::GESTURE_STATE_START) {
                // Start new pinch
                ndk_helper::Vec2 v1;
                ndk_helper::Vec2 v2;
                eng->pinch_detector_.GetPointers(v1, v2);
                eng->TransformPosition(v1);
                eng->TransformPosition(v2);
                eng->tap_camera_.BeginPinch(v1, v2);
            } else if (pinchState & ndk_helper::GESTURE_STATE_MOVE) {
                // Multi touch
                // Start new pinch
                ndk_helper::Vec2 v1;
                ndk_helper::Vec2 v2;
                eng->pinch_detector_.GetPointers(v1, v2);
                eng->TransformPosition(v1);
                eng->TransformPosition(v2);
                eng->tap_camera_.Pinch(v1, v2);
            }
        }
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
void Engine::onAppCmd(android_app *app, int32_t cmd) {
    auto* engine = static_cast<Engine*>(app->userData);
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
                engine->initWindow(app);
                engine->has_focus_ = true;
                engine->drawFrame();
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
            engine->resumeSensors();
            // Start animation
            engine->has_focus_ = true;
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("Processing APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_LOW_MEMORY:
            // Free up GL resources
            LOGI("Processing APP_CMD_LOW_MEMORY");
            engine->trimMemory();
            break;
        // Following cases executed in order when navigating away
        case APP_CMD_PAUSE:
            LOGI("Processing APP_CMD_PAUSE");
            break;
        case APP_CMD_LOST_FOCUS:
            // Can occur when looking at all apps scroll view
            LOGI("Processing APP_CMD_LOST_FOCUS");
            engine->suspendSensors();
            // Also stop animating.
            engine->has_focus_ = false;
            engine->drawFrame();
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            LOGI("Processing APP_CMD_TERM_WINDOW");
            engine->termWindow();
            engine->has_focus_ = false;
            break;
        case APP_CMD_STOP:
            LOGI("Processing APP_CMD_STOP");
            break;
        case APP_CMD_SAVE_STATE:
            LOGI("Processing APP_CMD_SAVE_STATE");
            break;
        // App
        case APP_CMD_DESTROY:
            LOGI("Processing APP_CMD_DESTROY");
            break;
        default:
            LOGW("Unhandled Command %d", cmd);
    }
}

//-------------------------------------------------------------------------
// Sensor handlers
//-------------------------------------------------------------------------
void Engine::initSensors() {
    sensor_manager_ = ndk_helper::AcquireASensorManagerInstance(app);
    accelerometer_sensor_ = ASensorManager_getDefaultSensor(sensor_manager_, ASENSOR_TYPE_ACCELEROMETER);
    sensor_event_queue_ = ASensorManager_createEventQueue(sensor_manager_, app->looper, LOOPER_ID_USER, nullptr, nullptr);
}

void Engine::processSensors(int32_t id) {
    // If a sensor has data, process it now.
    if (id == LOOPER_ID_USER) {
        if (accelerometer_sensor_ != nullptr) {
            ASensorEvent event;
            while (ASensorEventQueue_getEvents(sensor_event_queue_, &event, 1) > 0) {
                // Do Nothing
            }
        }
    }
//    else {
//        LOGW("Engine::ProcessSensors %d", id);
//    }
}

void Engine::resumeSensors() {
    // When our app gains focus, we start monitoring the accelerometer.
    if (accelerometer_sensor_ != nullptr) {
        ASensorEventQueue_enableSensor(sensor_event_queue_, accelerometer_sensor_);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(sensor_event_queue_, accelerometer_sensor_,
                                       (1000L / 60) * 1000);
    }
}

void Engine::suspendSensors() {
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if (accelerometer_sensor_ != nullptr) {
        ASensorEventQueue_disableSensor(sensor_event_queue_, accelerometer_sensor_);
    }
}


bool Engine::hasFocus() const {
    return has_focus_;
}

void Engine::TransformPosition(ndk_helper::Vec2 &vec) {
    vec = ndk_helper::Vec2(2.0f, 2.0f) * vec /
          ndk_helper::Vec2(static_cast<float>(gl_context_->getScreenWidth()),
                           static_cast<float>(gl_context_->getScreenHeight())) -
          ndk_helper::Vec2(1.f, 1.f);
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
    app->onAppCmd = Engine::onAppCmd;
    app->onInputEvent = Engine::onInputEvent;
    initSensors();

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

        processSensors(id);

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

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app *app) {
    LOGI("android_main()");
    Engine engine{app};
    engine.run();
}
