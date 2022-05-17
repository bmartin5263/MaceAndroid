//
// Created by Brandon on 5/13/22.
//

#ifndef MACE_MYENGINE_H
#define MACE_MYENGINE_H

#ifdef __ANDROID__
#include <android_native_app_glue.h>
#endif

class EngineImpl;
class EventSystem;
class MyEngine {
public:
#ifdef __ANDROID__
    static void Init(android_app* app);
#endif
    static void Launch();
    static EventSystem& EventSystem();

    /*
    static void launch(Scene *scene);
    static void pushScene(Scene *scene);

    static double deltaTime();

    static FileSystem& fileSystem();
    static Graphics& graphics();
    static Keyboard& keyboard();
    static UI& ui();
    static ResourcesImpl& resources();
     */

private:
    MyEngine() = default;
    MyEngine(MyEngine& other) = default;
    MyEngine& operator=(MyEngine& other) = default;
    ~MyEngine() = default;

    static EngineImpl& Instance();
};

#endif //MACE_MYENGINE_H
