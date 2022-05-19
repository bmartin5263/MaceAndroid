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

#include "rendering/TexturedTeapotRenderer.h"
#include "NDKHelper.h"

class Engine {
public:
    explicit Engine(android_app *app);

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

    void handleCmd(android_app* app, int32_t cmd);
    int32_t handleInputEvent(android_app* app, AInputEvent* event);

    static void OnAppCmd(android_app* app, int32_t cmd);
    static int32_t OnInputEvent(android_app* app, AInputEvent* event);

private:
    void showUI();
    void updateFPS();

    void transformPosition(Vec2 &vec);

    // Input handling
    void handleDrag(AInputEvent *event);
    void handlePinch(AInputEvent *event);
    inline void transformPosition(Vec2 &v1, Vec2 &v2);

    bool initialized;
    bool focused;

    android_app* app;

    TapCamera tapCamera;
    GLContext glContext;
    Clock engineClock;

    TexturedTeapotRenderer teapotRender;

    ndk_helper::DoubletapDetector doubletapDetector;
    ndk_helper::DragDetector dragDetector;
    ndk_helper::PinchDetector pinchDetector;
    ndk_helper::SensorManager sensorManager;
};

#endif //MACE_ENGINE_H
