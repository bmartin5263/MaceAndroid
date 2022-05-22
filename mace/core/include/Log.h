//
// Created by Brandon on 5/16/22.
//

#ifndef MACE_LOG_H
#define MACE_LOG_H

#include <string>
#include "Core.h"

MACE_START

class Log {
public:
    static void Debug(const char* fmt...);
    static void Debug(const char* fmt, const char* args...);

    static void Info(const char* fmt...);
    static void Info(const char* fmt, const char* args...);

    static void Warn(const char* fmt...);
    static void Warn(const char* fmt, const char* args...);

    static void Error(const char* fmt...);
    static void Error(const char* fmt, const char* args...);
};

MACE_END

#endif //MACE_LOG_H
