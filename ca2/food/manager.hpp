#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"

class Manager
{
private:
    Logger* logger;
    string name;
    vector<string> warehouse;
    vector<Item> items;

    void get_warehouse();
    void get_items();

    Item get_item(string warehouse_name);

public:
    Manager(Logger* _logger, string _name);
    ~Manager();

    Item calc_sum();
};

#endif