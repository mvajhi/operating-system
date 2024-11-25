#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"

class Manager
{
private:
    Logger* logger;

public:
    Manager(Logger* _logger) : logger(_logger) {};
    ~Manager();
    void create_food_manager();
    void create_warehouse_manager();
};

#endif