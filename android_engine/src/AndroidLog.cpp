//
// Created by Brandon on 5/16/22.
//

#include "Log.h"

#ifdef __ANDROID__
#include "JNIHelper.h"

void Log::Debug(const char *fmt...) {
    __android_log_print(ANDROID_LOG_DEBUG, ndk_helper::JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Debug(const char* fmt, const char* args...) {
    __android_log_print(ANDROID_LOG_DEBUG, ndk_helper::JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Info(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_INFO, ndk_helper::JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Info(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_INFO, ndk_helper::JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Warn(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_WARN, ndk_helper::JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Warn(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_WARN, ndk_helper::JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

void Log::Error(const char *fmt, ...) {
    __android_log_print(ANDROID_LOG_ERROR, ndk_helper::JNIHelper::GetInstance()->GetAppName(), "%s", fmt);
}

void Log::Error(const char *fmt, const char *args, ...) {
    __android_log_print(ANDROID_LOG_ERROR, ndk_helper::JNIHelper::GetInstance()->GetAppName(), fmt, args);
}

#endif