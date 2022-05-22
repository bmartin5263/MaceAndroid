#ifndef GLCONTEXT_H_
#define GLCONTEXT_H_

#include "EGL/egl.h"
#include "GLES2/gl2.h"
#include "android/log.h"

#include "JNIHelper.h"

MACE_START

/******************************************************************
 * OpenGL context handler
 * The class handles OpenGL and EGL context based on Android activity life cycle
 * The caller needs to call corresponding methods for each activity life cycle
 *events as it's done in sample codes.
 *
 * Also the class initializes OpenGL ES3 when the compatible driver is installed
 *in the device.
 * getGLVersion() returns 3.0~ when the device supports OpenGLES3.0
 *
 * Thread safety: OpenGL context is expecting used within dedicated single
 *thread,
 * thus GLContext class is not designed as a thread-safe
 */
class GLContext {
public:

    GLContext();
    GLContext(GLContext const&) = delete;
    void operator = (GLContext const&) = delete;
    virtual ~GLContext();

    void init(ANativeWindow* window);

    bool invalidate();
    EGLint resume(ANativeWindow* window);

    void suspend();

    EGLint swap();
    ANativeWindow* getANativeWindow() const { return window; };
    int32_t getScreenWidth() const { return width; }

    int32_t getScreenHeight() const { return height; }

private:
    // EGL configurations
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;

    // Screen parameters
    int32_t width;
    int32_t height;
    int32_t colorSize;
    int32_t depthSize;

    // Flags
    bool valid;
    float glVersion;

    void terminate();
    void initEGLSurface();
    void initEGLContext();

    int32_t getBufferColorSize() const { return colorSize; }
    int32_t getBufferDepthSize() const { return depthSize; }
    float getGLVersion() const { return glVersion; }
    EGLDisplay getDisplay() const { return display; }
    EGLSurface getSurface() const { return surface; }
};

MACE_END

#endif /* GLCONTEXT_H_ */
