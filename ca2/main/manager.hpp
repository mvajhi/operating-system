#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"

class Manager
{
private:
    Logger* logger;
    map<string, int> food_managers;
    map<string, int> warehouse_managers;

    void create_food_manager(string name);
    void create_warehouse_manager(string name);

public:
    Manager(Logger* _logger) : logger(_logger) {};
    ~Manager();
    void create_food_managers();
    void create_warehouse_managers();
};

#endif