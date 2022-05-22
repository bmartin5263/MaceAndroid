//
// Created by Brandon on 5/16/22.
//

#ifdef __ANDROID__

#include "Core.h"
#include "Log.h"
#include "JNIHelper.h"

using namespace mace;
using namespace mace::ndk;

void Log::Debug(const char *fmt...) {
    __android_log_print(ANDROID_LOG_DEBUG, JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Debug(const char* fmt, const char* args...) {
    __android_log_print(ANDROID_LOG_DEBUG, JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Info(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_INFO, JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Info(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_INFO, JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Warn(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_WARN, JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Warn(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_WARN, JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Error(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_ERROR, JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Error(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_ERROR, JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

#endif