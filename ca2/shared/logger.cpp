#include "logger.hpp"

void Logger::log(LogLevel level, const string &message)
{
    // ?INFO: true for showing logs, false for hiding logs
    map<LogLevel, bool> mask_log = {
        {DEBUG, false},
        {OTHER, false},
        {CREATE_PIPE, false},
        {CREATE_CHILD, true},
        {PIPE_READ, false},
        {PIPE_SEND, true},
        {PIPE_CLOSE, false},
        {CSV_READ, true},
        {RESULT, true},
        {END, true},
        };

    if (!mask_log[level])
    {
        return;
    }
    time_t now = time(0);
    tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", timeinfo);

    ostringstream logEntry;
    logEntry << "[" << timestamp << "] "
             << levelToString(level)
             << "(" << sender_name << ")"
             << ": " << message
             << endl;

    cout << logEntry.str();
}

string Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "\033[34mDEBUG\033[0m"; // Blue
    case OTHER:
        return "\033[32mOTHER\033[0m"; // Green
    case CREATE_PIPE:
        return "\033[33mCREATE_PIPE\033[0m"; // Yellow
    case CREATE_CHILD:
        return "\033[36mCREATE_CHILD\033[0m"; // Cyan
    case PIPE_READ:
        return "\033[33mPIPE_READ\033[0m"; // Yellow
    case PIPE_SEND:
        return "\033[33mPIPE_SEND\033[0m"; // Yellow
    case PIPE_CLOSE:
        return "\033[33mPIPE_CLOSE\033[0m"; // Yellow
    case CSV_READ:
        return "\033[90mCSV_READ\033[0m"; // Bright Black
    case RESULT:
        return "\033[35mRESULT\033[0m"; // Magenta
    case END:
        return "\033[31mEND\033[0m"; // Red
    default:
        return "\033[37mUNKNOWN\033[0m"; // White
    }
}