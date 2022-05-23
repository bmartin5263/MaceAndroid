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

#include "graphics/GLContext.h"

#include <cstring>
#include <unistd.h>

#include "graphics/gl3stub.h"
#include "Log.h"

MACE_START

GLContext::GLContext():
    window(nullptr),
    display(EGL_NO_DISPLAY),
    surface(EGL_NO_SURFACE),
    context(EGL_NO_CONTEXT),
    width(0),
    height(0),
    initialized(false)
{}

GLContext::~GLContext() {
    INFO("~GLContext()");
    terminate();
}

void GLContext::init(ANativeWindow* window) {
    INFO("Starting OpenGL ES");
    if (initialized) {
        return;
    }
    this->window = window;
    initEGLDisplay();
    initEGLContext();

    // Initialize GL state.
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    initialized = true;
}

void GLContext::reinit(ANativeWindow *window) {
    INFO("Resuming OpenGL ES")
    this->window = window;
    initEGLSurface();
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        ASSERT(false, "Unable to eglMakeCurrent");
    }
    // Note that screen size might have been changed
    glViewport(0, 0, width, height);
}

void GLContext::initEGLDisplay() {
    INFO("GLContext::initEGLDisplay()");
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    initEGLConfigs();
    initEGLSurface();
}

void GLContext::initEGLConfigs() {
    const EGLint attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    colorSize = 8;
    depthSize = 24;

    EGLint num_configs;
    eglChooseConfig(display, attributes, &config, 1, &num_configs);

    if (!num_configs) {
        // Fall back to 16bit depth buffer
        const EGLint attributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,  // Request opengl ES3.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };
        eglChooseConfig(display, attributes, &config, 1, &num_configs);
        depthSize = 16;
    }

    if (!num_configs) {
        ERROR("Unable to retrieve EGL config");
        ASSERT(false, "Unable to retrieve EGL config");
    }
}

void GLContext::initEGLSurface() {
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
}

void GLContext::initEGLContext() {
    INFO("GLContext::initEGLContext()");
    const EGLint attributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,  // Request opengl ES3.0
            EGL_NONE                        // done
    };
    context = eglCreateContext(display, config, nullptr, attributes);
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        ERROR("Unable to eglMakeCurrent");
        assert(false);
    }
}

EGLint GLContext::swap() {
    bool success = eglSwapBuffers(display, surface);
    if (!success) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            // Recreate surface
            initEGLDisplay();
            return EGL_SUCCESS;  // Still consider glContext is valid
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost!!
            ERROR("GLContext::swap() - context lost");
            terminate();
            initEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

void GLContext::terminate() {
    INFO("GLContext::terminate()");
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
    INFO("GLContext::resume()");
    if (!initialized) {
        init(window);
        return EGL_SUCCESS;
    }
    else {
        this->window = window;
        int32_t originalWidth = width;
        int32_t originalHeight = height;

        // Create surface
        surface = eglCreateWindowSurface(display, config, window, nullptr);
        eglQuerySurface(display, surface, EGL_WIDTH, &width);
        eglQuerySurface(display, surface, EGL_HEIGHT, &height);

        if (width != originalWidth || height != originalHeight) {
            // Screen resized
            INFO("GLContext::resume() - Screen resized");
        }

        if (eglMakeCurrent(display, surface, surface, context) == EGL_TRUE)
            return EGL_SUCCESS;

        EGLint err = eglGetError();
        INFO("Unable to eglMakeCurrent %d", err);

        if (err == EGL_CONTEXT_LOST) {
            // Recreate context
            INFO("Re-creating egl context");
            initEGLContext();
        } else {
            // Recreate surface
            terminate();
            initEGLDisplay();
            initEGLContext();
        }
        return err;
    }

}

void GLContext::suspend() {
    if (surface != EGL_NO_SURFACE) {
        INFO("GLContext::suspend()");
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
    else {
        INFO("GLContext::suspend() - no surface");
    }
}

bool GLContext::destroy() {
    INFO("Destroying GLContext");
    terminate();
    initialized = false;
    return true;
}

MACE_END
