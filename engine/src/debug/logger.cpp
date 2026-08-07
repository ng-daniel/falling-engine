#include "engine/debug/logger.h"

#include <iostream>

namespace {

    enum class LogLevel {
        Info,
        Warning,
        Error
    };

    std::string LogLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Info:
                return "\033[0;32mINFO\033[0m"; // GREEN
            case LogLevel::Warning:
                return "\033[0;33mWARN\033[0m"; // YELLOW
            case LogLevel::Error:
                return "\033[0;31mERRR\033[0m"; // RED
            default:
                return "IDEK";
        }
    }

    void Log(std::ostream& stream, LogLevel level, const char* location, std::string_view message) {
        stream << '[' << LogLevelToString(level) << ']';
        if (location != nullptr && location[0] != '\0') {
            stream << "\033[0;36m[" << location << "]\033[0m"; // CYAN
        }
        stream << ' ' << message;
        stream << std::endl;
    }

}

/**
 * @brief Logs an informational message.
 *
 * @param location The class where the log is generated.
 * @param message The message to log.
 */
void Logger::Info(const char* location, std::string_view message) {
    Log(std::cout, LogLevel::Info, location, message);
}

/**
 * @brief Logs a warning message.
 *
 * @param location The class where the log is generated.
 * @param message The message to log.
 */
void Logger::Warning(const char* location, std::string_view message) {
    Log(std::cerr, LogLevel::Warning, location, message);
}

/**
 * @brief Logs an error message.
 *
 * @param location The class where the log is generated.
 * @param message The message to log.
 */
void Logger::Error(const char* location, std::string_view message) {
    Log(std::cerr, LogLevel::Error, location, message);
}