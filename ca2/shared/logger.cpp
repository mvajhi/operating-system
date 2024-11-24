#include "logger.hpp"

Logger::Logger()
{
}

void Logger::log(LogLevel level, const string &message)
{
    time_t now = time(0);
    tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", timeinfo);

    ostringstream logEntry;
    logEntry << "[" << timestamp << "] "
             << levelToString(level) << ": " << message
             << endl;

    cout << logEntry.str();
}

string Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "\033[34mDEBUG\033[0m"; // Blue
    case INFO:
        return "\033[32mINFO\033[0m"; // Green
    case WARNING:
        return "\033[33mWARNING\033[0m"; // Yellow
    case ERROR:
        return "\033[31mERROR\033[0m"; // Red
    case CRITICAL:
        return "\033[35mCRITICAL\033[0m"; // Magenta
    default:
        return "\033[37mUNKNOWN\033[0m"; // White
    }
}