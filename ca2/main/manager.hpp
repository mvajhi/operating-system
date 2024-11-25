#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"
#include "unnamed_pipe.hpp"

class Manager
{
private:
    Logger* logger;
    map<string, int> food_managers;
    map<string, shared_ptr<UnnamedPipe>> warehouse_managers;

    void create_food_manager(string name);
    void create_warehouse_manager(string name);

    void store_warehouse_pipe(shared_ptr<UnnamedPipe> pipe, string &name);

    void setup_warehouse_child(shared_ptr<UnnamedPipe> pipe, string &name);

public:
    Manager(Logger* _logger) : logger(_logger) {};
    ~Manager();
    void create_food_managers();
    void create_warehouse_managers();
    void send_to_warehouse(string message);
};

#endif