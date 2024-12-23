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
    for (auto i : warehouse)
    {
        string fifo_name = FIFO_DIR + name + i;
        NamedPipe Npipe(logger, fifo_name);
        Npipe.remove_pipe();
    }
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
        items.push_back(get_item(i));
}

void Manager::create_pipes()
{
    pipe->send("done");
}

Item Manager::get_item(string warehouse_name)
{
    string messages = read_row_data(warehouse_name);

    vector<string> data = split(messages, SPLITER);

    Item result;
    result.count = stoi(data[0]);
    result.profit = stod(data[1]);
    result.remaining_cost = stoi(data[2]);
    return result;
}

string Manager::read_row_data(string &warehouse_name)
{
    const string fifo_name = FIFO_DIR + name + warehouse_name;
    nps[warehouse_name] = make_shared<NamedPipe>(logger, fifo_name);

    auto parent_pipe = nps[warehouse_name];
    parent_pipe->create();
    parent_pipe->open_for_reading();
    string message = parent_pipe->read_from_pipe();
    
    logger->log(DEBUG, "result " + warehouse_name + ": " + message);
    parent_pipe->close_pipe();
    return message;
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