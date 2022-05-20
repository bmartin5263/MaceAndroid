#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)
project(SpacegameActivity LANGUAGES C CXX)

# set up common compile options
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -fno-exceptions -frtti")

# commonDir = common folder
get_filename_component(commonDir ../common ABSOLUTE)
get_filename_component(engineDir ../engine ABSOLUTE)
get_filename_component(androidEngineDir ../android_engine ABSOLUTE)
get_filename_component(gameDir ../game ABSOLUTE)

# Clone stb_image if not exists
if ((NOT EXISTS ${commonDir}/stb) OR
    (NOT EXISTS ${commonDir}/stb/stb_image.h))
    execute_process(COMMAND git clone
                            https://github.com/nothings/stb.git
                            stb
                    WORKING_DIRECTORY ${commonDir})
endif()

## ndkHelperSrc = common/ndk_helper
#get_filename_component(ndkHelperSrc ${commonDir}/ndk_helper ABSOLUTE)
#
## build the ndk-helper library, additional directory must be specified
#add_subdirectory(${ndkHelperSrc}
#  ${commonDir}/ndkHelperBin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

#get_filename_component(engineSrc ${engineDir}/src ABSOLUTE)
#add_subdirectory(${engineSrc}
#  ${engineDir}/bin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

get_filename_component(androidEngineSrc ${androidEngineDir}/src/cpp ABSOLUTE)
#add_subdirectory(${androidEngineSrc}
#  ${androidEngineDir}/bin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

get_filename_component(gameSrc ${gameDir} ABSOLUTE)
#add_subdirectory(${gameSrc}
#  ${gameDir}/bin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

# now build app's shared lib
add_library(${PROJECT_NAME}
  SHARED
        src/main/cpp/AndroidMain.cpp
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
#target_include_directories(${PROJECT_NAME} PRIVATE ${commonDir})
#target_include_directories(${PROJECT_NAME} PRIVATE ${engineDir}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${androidEngineDir}/src/cpp)
#target_include_directories(${PROJECT_NAME} PRIVATE ${gameDir}/src)

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-function)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set_target_properties(${PROJECT_NAME}
  PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

# add lib dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC engine android_engine)
