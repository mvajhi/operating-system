#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "define.hpp"

class Logger
{
public:
    Logger(string _sender_name) : sender_name(_sender_name) {};

    void log(LogLevel level, const string &message);

private:
    string levelToString(LogLevel level);
    string sender_name;
};

#endif