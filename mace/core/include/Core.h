//
// Created by Brandon on 5/21/22.
//

#ifndef MACE_CORE_H
#define MACE_CORE_H

#define MACE_START namespace mace {
#define MACE_END }

#define MACE_NDK_START namespace mace { namespace ndk {
#define MACE_NDK_END }}

#define LOGI(...)                                                           \
  ((void)__android_log_print(                                               \
      ANDROID_LOG_INFO, mace::ndk::JNIHelper::GetInstance()->GetAppName(), \
      __VA_ARGS__))
#define LOGW(...)                                                           \
  ((void)__android_log_print(                                               \
      ANDROID_LOG_WARN, mace::ndk::JNIHelper::GetInstance()->GetAppName(), \
      __VA_ARGS__))
#define LOGE(...)                                                            \
  ((void)__android_log_print(                                                \
      ANDROID_LOG_ERROR, mace::ndk::JNIHelper::GetInstance()->GetAppName(), \
      __VA_ARGS__))

#endif //MACE_CORE_H
