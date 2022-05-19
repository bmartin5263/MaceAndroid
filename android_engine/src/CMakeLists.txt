# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("android_engine")

include(AndroidNdkModules)
android_ndk_import_module_native_app_glue()

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

get_filename_component(engineDir ${CMAKE_CURRENT_SOURCE_DIR}/../../engine ABSOLUTE)
get_filename_component(engineSrc ${engineDir}/src ABSOLUTE)
add_subdirectory(${engineSrc}
        ${engineDir}/bin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

add_library( # Sets the name of the library.
        android_engine

        # Sets the library as a shared library.
        STATIC

        GestureDetector.cpp
        JNIHelper.cpp
        SensorManager.cpp
        AndroidEngine.cpp
        AndroidLog.cpp
        AssetUtil.cpp
        Engine.cpp
)

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

set_target_properties(
        android_engine
        PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        INTERFACE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:android_engine,INCLUDE_DIRECTORIES>
)

target_include_directories(${PROJECT_NAME} PRIVATE ${engineDir}/src)

get_filename_component(commonDir ${CMAKE_CURRENT_SOURCE_DIR}/../../common ABSOLUTE)
target_include_directories(${PROJECT_NAME} PRIVATE ${commonDir})


target_link_libraries(android_engine PUBLIC
        native_app_glue
        GLESv3
        EGL
        log
        android
        atomic
)