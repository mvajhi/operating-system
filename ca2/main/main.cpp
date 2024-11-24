#include "define.hpp"
#include "logger.hpp"

int main()
{
    Logger logger("main");
    logger.log(LogLevel::INFO, "Hello, world!");

    return 0;
}