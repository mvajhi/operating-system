#include "define.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "unnamed_pipe.hpp"
#include "named_pipe.hpp"

vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

void send_to_food_manager(string food, Item result, Logger *logger, string program_name)
{
    string message = to_string(result.count) + "," + to_string(result.profit) + "," + to_string(result.remaining_cost);
    const string fifo_name = FIFO_DIR + food + program_name;
    NamedPipe child_pipe(logger, fifo_name);
    child_pipe.open_for_writing();
    child_pipe.write_to_pipe(message);
    child_pipe.close_pipe();
}

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

    UnnamedPipe pipe(&logger, read_fd, write_fd);

    string message = pipe.receive();
    vector<string> tokens = split(message, SPLITER);

    Manager manager(&logger, program_name);

    int profit = 0;
    for (auto food : tokens)
    {
        Item result = manager.get_total(food);
        send_to_food_manager(food, result, &logger, program_name);
        logger.log(DEBUG, food + "," + to_string(result.count) +
                              "," + to_string(result.profit) + "," + to_string(result.remaining_cost));
        profit += result.profit;
    }

    pipe.send(to_string(profit));

    logger.log(END, "End of " + program_name);
    return 0;
}