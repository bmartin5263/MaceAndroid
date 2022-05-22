//
// Created by Brandon on 5/16/22.
//

#ifdef __ANDROID__

#include "Core.h"
#include "Log.h"
#include "JNIHelper.h"

using namespace mace;
using namespace mace::ndk;

void Log::Debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_DEBUG, JNIHelper::GetInstance()->GetAppName(), fmt, args);
    va_end(args);
}

void Log::Info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, JNIHelper::GetInstance()->GetAppName(), fmt, args);
    va_end(args);
}

void Log::Warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_WARN, JNIHelper::GetInstance()->GetAppName(), fmt, args);
    va_end(args);
}

void Log::Error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, JNIHelper::GetInstance()->GetAppName(), fmt, args);
    va_end(args);
}

#endif