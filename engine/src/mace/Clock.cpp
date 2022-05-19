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

#include "Clock.h"

Clock::Clock():
    tickSum{0},
    tickSamples{0},
    lastTick{0.f},
    currentFps{0},
    lastTimeValue{0},
    tickIndex{0}
{
    for (auto& i : tickSamples) {
        i = 0;
    }
}

double Clock::updateTick(double currentTick) {
    // Update the sum by removing the old tick and adding the new one
    tickSum -= tickSamples[tickIndex];
    tickSum += currentTick;

    // Save the new one in the old one's position
    tickSamples[tickIndex] = currentTick;

    // Go to next array position, or back to the start
    tickIndex = (tickIndex + 1) % SAMPLE_COUNT;

    return (static_cast<double>(tickSum) / SAMPLE_COUNT);
}

bool Clock::update(float &fFPS) {
    timeval timeOfDay = GetTimeOfDay();

    double time = GetCurrentTime();
    double tick = time - lastTick;
    double d = updateTick(tick);
    lastTick = time;

    if (timeOfDay.tv_sec - lastTimeValue >= 1) {
        currentFps = 1.f / static_cast<float>(d);
        lastTimeValue = timeOfDay.tv_sec;
        fFPS = currentFps;
        return true;
    } else {
        fFPS = currentFps;
        return false;
    }
}
