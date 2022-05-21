# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("mace")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

get_filename_component(coreDir core ABSOLUTE)
get_filename_component(coreLib ${coreDir}/lib ABSOLUTE)

if ((NOT EXISTS ${coreLib}/stb) OR
(NOT EXISTS ${coreLib}/stb/stb_image.h))
    execute_process(COMMAND git clone
            https://github.com/nothings/stb.git
            stb
            WORKING_DIRECTORY ${coreLib})
endif()

IF(ANDROID)
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()
    add_library( # Sets the name of the library.
            mace

            # Sets the library as a shared library.
            STATIC

            ndk/cpp/Bridge.cpp
            ndk/cpp/GestureDetector.cpp
            ndk/cpp/JNIHelper.cpp
            ndk/cpp/SensorManager.cpp
            ndk/cpp/AndroidEngine.cpp
            ndk/cpp/AndroidLog.cpp
            ndk/cpp/AssetUtil.cpp
            ndk/cpp/Engine.cpp

            core/src/Core.cpp
            core/src/MyEngine.cpp
            core/src/Clock.cpp
            core/src/EventSystem.cpp
            core/src/gl3stub.cpp
            core/src/GLContext.cpp
            core/src/Interpolator.cpp
            core/src/EngineImpl.cpp
            core/src/Event.cpp
            core/src/Graphics.cpp
            core/src/TeapotRenderer.cpp
            core/src/TexturedTeapotRenderer.cpp
            core/src/Shader.cpp
            core/src/TapCamera.cpp
            core/src/vecmath.cpp
            core/src/Texture.cpp
    )
    set_target_properties(
            mace
            PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
            INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:mace,INCLUDE_DIRECTORIES>
    )
    target_link_libraries(mace PUBLIC
            native_app_glue
            GLESv3
            EGL
            log
            android
            atomic
    )
ENDIF()

target_include_directories(mace PUBLIC core/include)
target_include_directories(mace PRIVATE ${coreLib})
target_include_directories(mace PUBLIC ndk/include)
target_include_directories(mace PUBLIC core/pinclude)