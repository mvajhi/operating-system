#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"
#include "unnamed_pipe.hpp"
#include "named_pipe.hpp"

class Manager
{
private:
    Logger* logger;
    map<string, shared_ptr<UnnamedPipe>> food_managers;
    map<string, shared_ptr<UnnamedPipe>> warehouse_managers;
    vector<string> v_w_names;
    string w_names;


    void create_food_manager(string name);
    void setup_food_child(shared_ptr<UnnamedPipe> pipe, string &name);
    void store_food_pipe(shared_ptr<UnnamedPipe> pipe, string &name);

    void create_warehouse_manager(string name);
    void setup_warehouse_child(shared_ptr<UnnamedPipe> pipe, string &name);
    void store_warehouse_pipe(shared_ptr<UnnamedPipe> pipe, string &name);

public:
    Manager(Logger* _logger);
    ~Manager();
    void create_food_managers(string foods_names);
    void send_to_warehouse(string message);
    void show_items();
};

#endif