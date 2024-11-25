#include "define.hpp"
#include "logger.hpp"
#include "manager.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    string program_name = argv[1];
    string pipe_name = argv[2];

    Logger logger(program_name);
    logger.log(PIPE_READ, "Reading from pipe " + pipe_name);

    Manager manager(&logger, program_name);

    return 0;
}