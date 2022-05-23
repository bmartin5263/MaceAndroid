#include "GestureDetector.h"

MACE_NDK_START

void GestureDetector::setConfiguration(AConfiguration* config) {
    dpFactor = 160.f / static_cast<float>(AConfiguration_getDensity(config));
}

GestureState TapDetector::detect(const AInputEvent* motion_event) {
    if (AMotionEvent_getPointerCount(motion_event) > 1) {
        // Only support single touch
        return false;
    }

    int32_t action = AMotionEvent_getAction(motion_event);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            downPointerId = AMotionEvent_getPointerId(motion_event, 0);
            downX = AMotionEvent_getX(motion_event, 0);
            downY = AMotionEvent_getY(motion_event, 0);
            break;
        case AMOTION_EVENT_ACTION_UP: {
            int64_t eventTime = AMotionEvent_getEventTime(motion_event);
            int64_t downTime = AMotionEvent_getDownTime(motion_event);
            if (eventTime - downTime <= TAP_TIMEOUT) {
                if (downPointerId == AMotionEvent_getPointerId(motion_event, 0)) {
                    float x = AMotionEvent_getX(motion_event, 0) - downX;
                    float y = AMotionEvent_getY(motion_event, 0) - downY;
                    if (x * x + y * y < TOUCH_SLOP * TOUCH_SLOP * dpFactor) {
                        INFO("TapDetector: Tap detected");
                        return GESTURE_STATE_ACTION;
                    }
                }
            }
            break;
        }
    }
    return GESTURE_STATE_NONE;
}

GestureState DoubletapDetector::detect(const AInputEvent* motionEvent) {
    if (AMotionEvent_getPointerCount(motionEvent) > 1) {
        // Only support single double tap
        return false;
    }

    bool tapDetected = tapDetector.detect(motionEvent);

    int32_t action = AMotionEvent_getAction(motionEvent);
    unsigned int flags = action & AMOTION_EVENT_ACTION_MASK;
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN: {
            int64_t eventTime = AMotionEvent_getEventTime(motionEvent);
            if (eventTime - lastTapTime <= DOUBLE_TAP_TIMEOUT) {
                float x = AMotionEvent_getX(motionEvent, 0) - lastX;
                float y = AMotionEvent_getY(motionEvent, 0) - lastY;
                if (x * x + y * y < DOUBLE_TAP_SLOP * DOUBLE_TAP_SLOP * dpFactor) {
                    INFO("DoubletapDetector: Doubletap detected");
                    return GESTURE_STATE_ACTION;
                }
            }
            break;
        }
        case AMOTION_EVENT_ACTION_UP:
            if (tapDetected) {
                lastTapTime = AMotionEvent_getEventTime(motionEvent);
                lastX = AMotionEvent_getX(motionEvent, 0);
                lastY = AMotionEvent_getY(motionEvent, 0);
            }
            break;
    }
    return GESTURE_STATE_NONE;
}

void DoubletapDetector::setConfiguration(AConfiguration* config) {
    GestureDetector::setConfiguration(config);
    tapDetector.setConfiguration(config);
}

//--------------------------------------------------------------------------------
// PinchDetector
//--------------------------------------------------------------------------------

int32_t GestureDetector::FindIndex(const AInputEvent* event, int32_t id) {
    auto count = AMotionEvent_getPointerCount(event);
    for (auto i = 0; i < count; ++i) {
        if (id == AMotionEvent_getPointerId(event, i)) {
            return i;
        }
    }
    return -1;
}

GestureState PinchDetector::detect(const AInputEvent* event) {
    GestureState ret = GESTURE_STATE_NONE;
    int32_t action = AMotionEvent_getAction(event);
    uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
    this->event = event;

    int32_t count = AMotionEvent_getPointerCount(event);
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            pointers.push_back(AMotionEvent_getPointerId(event, 0));
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN: {
            int32_t iIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                                                                AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointers.push_back(AMotionEvent_getPointerId(event, iIndex));
            if (count == 2) {
                // Start new pinch
                ret = GESTURE_STATE_START;
            }
        } break;
        case AMOTION_EVENT_ACTION_UP:
            pointers.pop_back();
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP: {
            int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                                                               AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int32_t released_pointer_id = AMotionEvent_getPointerId(event, index);

            std::vector<int32_t>::iterator it = pointers.begin();
            std::vector<int32_t>::iterator it_end = pointers.end();
            int32_t i = 0;
            for (; it != it_end; ++it, ++i) {
                if (*it == released_pointer_id) {
                    pointers.erase(it);
                    break;
                }
            }

            if (i <= 1) {
                // Reset pinch or drag
                if (count != 2) {
                    // Start new pinch
                    ret = GESTURE_STATE_START | GESTURE_STATE_END;
                }
            }
        } break;
        case AMOTION_EVENT_ACTION_MOVE:
            switch (count) {
                case 1:
                    break;
                default:
                    // Multi touch
                    ret = GESTURE_STATE_MOVE;
                    break;
            }
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            break;
    }

    return ret;
}

bool PinchDetector::getPointers(Vec2& v1, Vec2& v2) {
    if (pointers.size() < 2) return false;

    int32_t index = GestureDetector::FindIndex(event, pointers[0]);
    if (index == -1) return false;

    float x = AMotionEvent_getX(event, index);
    float y = AMotionEvent_getY(event, index);

    index = FindIndex(event, pointers[1]);
    if (index == -1) return false;

    float x2 = AMotionEvent_getX(event, index);
    float y2 = AMotionEvent_getY(event, index);

    v1 = Vec2(x, y);
    v2 = Vec2(x2, y2);

    return true;
}

//--------------------------------------------------------------------------------
// DragDetector
//--------------------------------------------------------------------------------

GestureState DragDetector::detect(const AInputEvent* event) {
    GestureState ret = GESTURE_STATE_NONE;
    int32_t action = AMotionEvent_getAction(event);
    int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                                                       AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
    this->event = event;

    int32_t count = static_cast<int32_t>(AMotionEvent_getPointerCount(event));
    switch (flags) {
        case AMOTION_EVENT_ACTION_DOWN:
            pointers.push_back(AMotionEvent_getPointerId(event, 0));
            ret = GESTURE_STATE_START;
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pointers.push_back(AMotionEvent_getPointerId(event, index));
            break;
        case AMOTION_EVENT_ACTION_UP:
            pointers.pop_back();
            ret = GESTURE_STATE_END;
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP: {
            int32_t released_pointer_id = AMotionEvent_getPointerId(event, index);

            auto it = pointers.begin();
            auto it_end = pointers.end();
            int32_t i = 0;
            for (; it != it_end; ++it, ++i) {
                if (*it == released_pointer_id) {
                    pointers.erase(it);
                    break;
                }
            }

            if (i <= 1) {
                // Reset pinch or drag
                if (count == 2) {
                    ret = GESTURE_STATE_START;
                }
            }
            break;
        }
        case AMOTION_EVENT_ACTION_MOVE:
            switch (count) {
                case 1:
                    // Drag
                    ret = GESTURE_STATE_MOVE;
                    break;
                default:
                    break;
            }
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            break;
    }

    return ret;
}

bool DragDetector::getPointer(Vec2& v) {
    if (pointers.empty()) {
        return false;
    }

    int32_t iIndex = GestureDetector::FindIndex(event, pointers[0]);
    if (iIndex == -1) {
        return false;
    }

    float x = AMotionEvent_getX(event, iIndex);
    float y = AMotionEvent_getY(event, iIndex);

    v = Vec2(x, y);

    return true;
}

MACE_NDK_END
