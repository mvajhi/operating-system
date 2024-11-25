#include "define.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "unnamed_pipe.hpp"

vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void send_to_food_manager(string food, Item result, Logger* logger, string program_name)
{
    string response = to_string(result.count) + "," + to_string(result.profit) + "," + to_string(result.remaining_cost);
    string fifo_name = FIFO_DIR + food + program_name;
    NamedPipe Npipe(logger, fifo_name);
    Npipe.open_for_writing();
    Npipe.write_to_pipe(response);
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
        logger.log(DEBUG, food + "," + to_string(result.count) + "," + to_string(result.profit) + "," + to_string(result.remaining_cost));
        profit += result.profit;
    }

    pipe.send(to_string(profit));

    // auto tmp = manager.get_total("shekar");
    // logger.log(DEBUG, "Total: " + to_string(tmp.count) + " " + to_string(tmp.profit) + " " + to_string(tmp.remaining_cost));
    // tmp = manager.get_total("berenj");
    // logger.log(DEBUG, "Total: " + to_string(tmp.count) + " " + to_string(tmp.profit) + " " + to_string(tmp.remaining_cost));

    return 0;
}