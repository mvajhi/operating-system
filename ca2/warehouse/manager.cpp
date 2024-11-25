#include "manager.hpp"

void Manager::create_io_log()
{
    string filename = "./" + DATA_DIR + name + ".csv";
    csv_reader reader(filename);
    CSV data = reader.read();
    logger->log(CSV_READ, "Read " + to_string(data.size()) + " rows from " + filename);
    for (vector<string> row : data)
    {
        Transaction transaction;
        transaction.count = stoi(row[CSV_INDEX_COUNT]);
        transaction.price = stoi(row[CSV_INDEX_PRICE]);
        transaction.type = CSV_INPUT == row[CSV_INDEX_TYPE];
        io_log[row[CSV_INDEX_NAME]].push_back(transaction);
        // logger->log(DEBUG, "Transaction: " + row[CSV_INDEX_NAME] + " " + to_string(transaction.count) + " " + to_string(transaction.price) + " " + to_string(transaction.type));
    }
    logger->log(OTHER, "Created io log for " + name);
}

Item Manager::create_item(pair<const string, vector<Transaction>> entry)
{
    Item item;
    item.count = 0;
    item.profit = 0;
    item.remaining_cost = 0;
    calc_profit(entry, item);
    calc_cost_and_count(entry, item);
    return item;
}

Item Manager::get_total(string name)
{
    if (io_log.find(name) == io_log.end())
    {
        Item item;
        item.count = 0;
        item.profit = 0;
        item.remaining_cost = 0;
        return item;
    }
    return create_item(pair<string, vector<Transaction>>(name, io_log[name]));
}

void Manager::calc_cost_and_count(const pair<const string, vector<Transaction>> &entry, Item &item)
{
    // logger->log(DEBUG, "Item: " + entry.first);
    for (Transaction transaction : entry.second)
    {
        if (transaction.type == INPUT)
        {
            // logger->log(DEBUG, "Transaction: " + to_string(transaction.count) + " " + to_string(transaction.price) + " " + to_string(transaction.type));
            item.count += transaction.count;
            item.remaining_cost += transaction.count * transaction.price;
        }
    }
    // logger->log(DEBUG, "Item: " + entry.first + " Count: " + to_string(item.count) + " Remaining Cost: " + to_string(item.remaining_cost));
}

void Manager::calc_profit(pair<const string, vector<Transaction>> &entry, Item &item)
{
    for (Transaction &i : entry.second)
    {
        if (i.type == INPUT)
            continue;

        for (Transaction &j : entry.second)
        {
            if (j.type == OUTPUT || j.count == 0)
                continue;
            if (i.count == 0)
                break;
            int count = min(i.count, j.count);
            i.count -= count;
            j.count -= count;
            item.profit += count * (i.price - j.price);
            // logger->log(DEBUG, "Item: " + entry.first + " Profit: " + to_string(item.profit) + " i count:" + to_string(i.count) + " j count:" + to_string(j.count));
        }
    }
}

Manager::Manager(Logger *_logger, string _name)
    : logger(_logger), name(_name)
{
    create_io_log();
    // TODO create pipes
}

Manager::~Manager()
{
    // TODO close all pipes
}
