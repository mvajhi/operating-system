#include "manager.hpp"

Manager::Manager(Logger *_logger, string _name)
    : logger(_logger), name(_name)
{
    get_warehouse();
    get_items();
}

Manager::~Manager()
{
    // TODO close all pipes
}

void Manager::get_warehouse()
{
    // TODO get with pipe
    logger->log(PIPE_SEND, "I want to get warehouse list");
    logger->log(PIPE_READ, "Reading from pipe");

    warehouse.push_back("warehouse1");
    warehouse.push_back("warehouse2");
    warehouse.push_back("warehouse3");
}

void Manager::get_items()
{
    for (auto i : warehouse)
    {
        items.push_back(get_item(i));
        logger->log(DEBUG, "Item " + i + " added to items");
    }
}

Item Manager::get_item(string warehouse_name)
{
    // TODO connect to pipe

    // TODO send my name and get data
    logger->log(PIPE_SEND, "Sending " + warehouse_name + " to pipe");

    logger->log(PIPE_READ, "Reading from pipe");
    logger->log(PIPE_CLOSE, "Closing pipe");
    // TODO convert data to Item
    Item result;
    result.count = 10;
    result.profit = 20;
    result.remaining_cost = 15;
    return result;
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