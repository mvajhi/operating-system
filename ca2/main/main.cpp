#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"
#include "manager.hpp"

int main()
{
    Logger logger("main");
    Manager manager(&logger);

    manager.create_warehouse_manager();

    return 0;
}