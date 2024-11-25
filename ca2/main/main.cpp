#include "define.hpp"
#include "logger.hpp"

int main()
{

    int pid, pip[2];
    char instring[20];

    pipe(pip);

    pid = fork();
    if (pid == 0)
    {
        Logger logger("child");
        logger.log(LogLevel::INFO, "sending message...");
        write(pip[1], "Hi Mom!", 8);
        logger.log(LogLevel::INFO, "massage sent");
    }
    else
    {
        Logger logger("main");
        read(pip[0], instring, 8);
        cout << "Received: " << instring << endl;
        logger.log(LogLevel::INFO, string(instring));
    }

    return 0;
}