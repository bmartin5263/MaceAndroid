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

#ifndef GESTUREDETECTOR_H_
#define GESTUREDETECTOR_H_

#include <vector>

#include <android/sensor.h>
#include <android/log.h>
#include "JNIHelper.h"
#include "vecmath.h"

namespace ndk_helper {

const int32_t DOUBLE_TAP_TIMEOUT = 300 * 1000000;
const int32_t TAP_TIMEOUT = 180 * 1000000;
const int32_t DOUBLE_TAP_SLOP = 100;
const int32_t TOUCH_SLOP = 8;

enum {
    GESTURE_STATE_NONE = 0,
    GESTURE_STATE_START = 1,
    GESTURE_STATE_MOVE = 2,
    GESTURE_STATE_END = 4,
    GESTURE_STATE_ACTION = (GESTURE_STATE_START | GESTURE_STATE_END),
};
typedef int32_t GestureState;

/******************************************************************
 * Base class of Gesture Detectors
 * GestureDetectors handles input events and detect gestures
 * Note that different detectors may detect gestures with an event at
 * same time. The caller needs to manage gesture priority accordingly
 *
 */
class GestureDetector {
public:
    virtual ~GestureDetector() = default;

    virtual void setConfiguration(AConfiguration *config);
    virtual GestureState detect(const AInputEvent *motionEvent) = 0;

protected:
    static int32_t FindIndex(const AInputEvent *event, int32_t id);

    float dpFactor{1.f};
};

class TapDetector : public GestureDetector {
public:
    /**
     * Returns GESTURE_STATE_ACTION when a tap gesture is detected
     */
    GestureState detect(const AInputEvent *motion_event) override;

private:
    int32_t downPointerId{};
    float downX{};
    float downY{};
};

class DoubletapDetector : public GestureDetector {
public:
    /**
     * Returns GESTURE_STATE_ACTION when a double-tap gesture is detected
     */
    GestureState detect(const AInputEvent *motionEvent) override;
    void setConfiguration(AConfiguration *config) override;

private:
    TapDetector tapDetector{};
    int64_t lastTapTime{};
    float lastX{};
    float lastY{};
};

/******************************************************************
 * Double gesture detector
 * Returns pinch gesture state when a pinch gesture is detected
 * The class handles multiple touches more than 2
 * When the finger 1,2,3 are tapped and then finger 1 is released,
 * the detector start new pinch gesture with finger 2 & 3.
 */
class PinchDetector : public GestureDetector {
public:
    GestureState detect(const AInputEvent *event) override;
    bool getPointers(Vec2 &v1, Vec2 &v2);

private:
    const AInputEvent *event{nullptr};
    std::vector<int32_t> pointers{};
};

/******************************************************************
 * drag gesture detector
 * Returns drag gesture state when a drag-tap gesture is detected
 *
 */
class DragDetector : public GestureDetector {
public:
    GestureState detect(const AInputEvent *event) override;
    bool getPointer(Vec2 &v);

private:
    const AInputEvent *event{nullptr};
    std::vector<int32_t> pointers{};
};

}  // namespace ndkHelper
#endif /* GESTUREDETECTOR_H_ */
