#include "define.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "unnamed_pipe.hpp"
#include "named_pipe.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    string program_name = argv[3];
    Logger logger(program_name);
    int read_fd = atoi(argv[1]);
    int write_fd = atoi(argv[2]);

    shared_ptr<UnnamedPipe> pipe = make_shared<UnnamedPipe>(&logger, read_fd, write_fd);

    Manager manager(&logger, program_name, pipe);

    Item sum = manager.calc_sum();

    logger.log(RESULT, "Total count: " + to_string(sum.count) + " Total profit: " +
                           to_string(sum.profit) + " Total remaining cost: " + to_string(sum.remaining_cost));

    logger.log(END, "End of " + program_name);
    return 0;
}