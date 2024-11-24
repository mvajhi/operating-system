#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "define.hpp"

class Logger
{
public:
    Logger();

    void log(LogLevel level, const string &message);

private:
    string levelToString(LogLevel level);
};

#endif