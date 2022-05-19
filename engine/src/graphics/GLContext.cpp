/*
 * Copyright 2013 The Android Open Source Project
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

#include "GLContext.h"

#include <cstring>
#include <unistd.h>

#include "gl3stub.h"
#include "Log.h"

GLContext::GLContext():
    window(nullptr),
    display(EGL_NO_DISPLAY),
    surface(EGL_NO_SURFACE),
    context(EGL_NO_CONTEXT),
    width(0),
    height(0),
    valid(false)
{}

GLContext::~GLContext() {
    Log::Warn("~GLContext()");
    terminate();
}

void GLContext::init(ANativeWindow* window) {
    Log::Warn("GLContext::init()");
    if (valid) {
        return;
    }
    this->window = window;
    initEGLSurface();
    initEGLContext();
    valid = true;
}

void GLContext::initEGLSurface() {
    Log::Warn("GLContext::initEGLSurface()");
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,
            EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,
            8,
            EGL_GREEN_SIZE,
            8,
            EGL_RED_SIZE,
            8,
            EGL_DEPTH_SIZE,
            24,
            EGL_NONE
    };
    colorSize = 8;
    depthSize = 24;

    EGLint num_configs;
    eglChooseConfig(display, attribs, &config, 1, &num_configs);

    if (!num_configs) {
        // Fall back to 16bit depth buffer
        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE,
                EGL_OPENGL_ES3_BIT,  // Request opengl ES3.0
                EGL_SURFACE_TYPE,
                EGL_WINDOW_BIT,
                EGL_BLUE_SIZE,
                8,
                EGL_GREEN_SIZE,
                8,
                EGL_RED_SIZE,
                8,
                EGL_DEPTH_SIZE,
                16,
                EGL_NONE
        };
        eglChooseConfig(display, attribs, &config, 1, &num_configs);
        depthSize = 16;
    }

    if (!num_configs) {
        LOGE("Unable to retrieve EGL config");
        assert(false);
    }

    surface = eglCreateWindowSurface(display, config, window, nullptr);
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void GLContext::initEGLContext() {
    Log::Warn("GLContext::initEGLContext()");
    const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,  // Request opengl ES2.0
            EGL_NONE                        // done
    };
    context = eglCreateContext(display, config, nullptr, context_attribs);
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        assert(false);
    }
}

EGLint GLContext::swap() {
    bool success = eglSwapBuffers(display, surface);
    if (!success) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            // Recreate surface
            initEGLSurface();
            return EGL_SUCCESS;  // Still consider glContext is valid
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost!!
            LOGE("GLContext::swap() - context lost");
            terminate();
            initEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

void GLContext::terminate() {
    Log::Warn("GLContext::terminate()");
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }

        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    window = nullptr;
}

EGLint GLContext::resume(ANativeWindow* window) {
    Log::Warn("GLContext::resume()");
    if (!valid) {
        init(window);
        return EGL_SUCCESS;
    }

    this->window = window;
    int32_t original_widhth = width;
    int32_t original_height = height;

    // Create surface
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

    if (width != original_widhth || height != original_height) {
        // Screen resized
        LOGW("GLContext::resume() - Screen resized");
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_TRUE)
        return EGL_SUCCESS;

    EGLint err = eglGetError();
    LOGW("Unable to eglMakeCurrent %d", err);

    if (err == EGL_CONTEXT_LOST) {
        // Recreate context
        LOGI("Re-creating egl context");
        initEGLContext();
    } else {
        // Recreate surface
        terminate();
        initEGLSurface();
        initEGLContext();
    }

    return err;
}

void GLContext::suspend() {
    if (surface != EGL_NO_SURFACE) {
        Log::Warn("GLContext::suspend()");
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
    else {
        Log::Warn("GLContext::suspend() - no surface");
    }
}

bool GLContext::invalidate() {
    Log::Warn("GLContext::invalidate()");
    terminate();
    valid = false;
    return true;
}
