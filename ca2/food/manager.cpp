#include "manager.hpp"

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

Manager::Manager(Logger *_logger, string _name, shared_ptr<UnnamedPipe> _pipe)
    : logger(_logger), name(_name), pipe(_pipe)
{
    get_warehouse();
    create_pipes();
    get_items();
}

Manager::~Manager()
{
    // TODO close all pipes
}

void Manager::get_warehouse()
{
    string message = pipe->receive();
    warehouse = split(message, SPLITER);
    logger->log(DEBUG, "sample 3 Warehouse received: " + warehouse[2]);
}

void Manager::get_items()
{
    for (auto i : warehouse)
    {
        items.push_back(get_item(i));
        logger->log(DEBUG, "Item " + i + " added to items");
    }
}

void Manager::create_pipes()
{
    for (auto i : warehouse)
    {
        string fifo_name = FIFO_DIR + name + i;
        NamedPipe Npipe(logger, fifo_name);
        Npipe.create();
    }
}

Item Manager::get_item(string warehouse_name)
{
    string messages = read_row_data(warehouse_name);
    // TODO convert data to Item
    vector<string> data = split(messages, SPLITER);
    Item result;
    result.count = stoi(data[0]);
    result.profit = stoi(data[1]);
    result.remaining_cost = stoi(data[2]);
    return result;
}

string Manager::read_row_data(string &warehouse_name)
{
    string fifo_name = FIFO_DIR + name + warehouse_name;
    NamedPipe Npipe(logger, fifo_name);
    Npipe.open_for_reading();
    return Npipe.read_from_pipe();
}

Item Manager::calc_sum()
{
    Item result;
    result.count = 0;
    result.profit = 0;
    result.remaining_cost = 0;

    for (auto i : items)
    {
        result.count += i.count;
        result.profit += i.profit;
        result.remaining_cost += i.remaining_cost;
    }

    return result;
}