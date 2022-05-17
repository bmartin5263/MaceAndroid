//
// Created by Brandon on 5/16/22.
//

#ifndef MACE_LOG_H
#define MACE_LOG_H

#include <string>

class Log {
public:
    static void Debug(const char* fmt...);
    static void Debug(const char* fmt, const char* args...);

    static void Info(const char* fmt...);
    static void Info(const char* fmt, const char* args...);

    static void Warn(const char* fmt...);
    static void Warning(const char* fmt, const char* args...);

    static void Error(const char* fmt...);
    static void Error(const char* fmt, const char* args...);
};

#endif //MACE_LOG_H
