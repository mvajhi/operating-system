#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "unnamed_pipe.hpp"
#include "named_pipe.hpp"

class Manager
{
private:
    Logger* logger;
    string name;
    vector<string> warehouse;
    vector<Item> items;
    shared_ptr<UnnamedPipe> pipe;

    void get_warehouse();
    void get_items();
    void create_pipes();

    Item get_item(string warehouse_name);

    string read_row_data(string &warehouse_name);

public:
    Manager(Logger* _logger, string _name, shared_ptr<UnnamedPipe> _pipe);
    ~Manager();

    Item calc_sum();
};

#endif