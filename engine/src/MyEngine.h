//
// Created by Brandon on 5/13/22.
//

#ifndef MACE_MYENGINE_H
#define MACE_MYENGINE_H

#ifdef __ANDROID__
#include <android_native_app_glue.h>
#endif

class MyEngine {
public:
#ifdef __ANDROID__
    explicit MyEngine(android_app *app);
#endif

private:

};


#endif //MACE_MYENGINE_H
