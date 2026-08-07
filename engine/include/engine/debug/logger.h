#ifndef ENGINE_DEBUG_LOGGER_H
#define ENGINE_DEBUG_LOGGER_H

#include <string_view>

class Logger {
public:
    static void Info(const char* location, std::string_view message);
    static void Warning(const char* location, std::string_view message);
    static void Error(const char* location, std::string_view message);
};

#endif // ENGINE_DEBUG_LOGGER_H