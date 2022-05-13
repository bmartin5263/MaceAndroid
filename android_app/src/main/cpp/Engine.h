//
// Created by Brandon on 5/12/22.
//

#ifndef MACE_ENGINE_H
#define MACE_ENGINE_H

#include <jni.h>
#include <cerrno>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>

#include "TexturedTeapotRender.h"
#include "NDKHelper.h"

const char* HELPER_CLASS_NAME = "dev/bdon/helper/NDKHelper";

struct android_app;
class Engine {
public:
    explicit Engine(android_app *app);
    ~Engine() = default;

    void run();
    bool update();
    void draw();

    int initWindow(android_app *app);
    void loadResources();
    void unloadResources();
    void drawFrame();
    void termWindow();
    void trimMemory();
    bool hasFocus() const;
    void initSensors();
    void processSensors(int32_t id);
    void suspendSensors();
    void resumeSensors();

    static void onAppCmd(android_app* app, int32_t cmd);
    static int32_t onInputEvent(android_app* app, AInputEvent* event);

private:
    void showUI();
    void updateFPS();

    void TransformPosition(ndk_helper::Vec2 &vec);

    TexturedTeapotRender renderer_;
    ndk_helper::GLContext *gl_context_;

    bool initialized_resources_;
    bool has_focus_;
    ndk_helper::DoubletapDetector doubletap_detector_;
    ndk_helper::PinchDetector pinch_detector_;

    ndk_helper::DragDetector drag_detector_;

    ndk_helper::PerfMonitor monitor_;

    ndk_helper::TapCamera tap_camera_;
    android_app *app;
    ASensorManager *sensor_manager_;

    const ASensor *accelerometer_sensor_;

    ASensorEventQueue *sensor_event_queue_;
};

#endif //MACE_ENGINE_H
