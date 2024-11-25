#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"

class Manager
{
private:
    Logger* logger;
    string name;
    map<string, vector<Transaction>> io_log;
    map<string, Item> total;

    void create_io_log();
    void create_total();

    void create_item(pair<const string, vector<Transaction>> entry);

    void calc_cost_and_count(const pair<const string, vector<Transaction>> &entry, Item &item);
    void calc_profit(pair<const string, vector<Transaction>> &entry, Item &item);

public:
    Manager(Logger* _logger, string _name);
    ~Manager();

};

#endif