# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("engine")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        engine

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        src/mace/MyEngine.cpp
        src/mace/Clock.cpp
        src/mace/EventSystem.cpp
        src/mace/graphics/gl3stub.cpp
        src/mace/graphics/GLContext.cpp
        src/mace/graphics/Interpolator.cpp
        src/mace/engine/EngineImpl.cpp
        src/mace/events/Event.cpp
        src/mace/graphics/Graphics.cpp
        src/mace/rendering/TeapotRenderer.cpp
        src/mace/rendering/TexturedTeapotRenderer.cpp
        src/mace/graphics/Shader.cpp
        src/mace/rendering/TapCamera.cpp
        src/mace/math/vecmath.cpp
        src/mace/graphics/Texture.cpp
)

target_link_libraries(engine android_engine)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library( # Sets the name of the path variable.
#        log-lib
#
#        # Specifies the name of the NDK library that
#        # you want CMake to locate.
#        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#target_link_libraries( # Specifies the target library.
#        engine
#
#        # Links the target library to the log library
#        # included in the NDK.
#        ${log-lib})