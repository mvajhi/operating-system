#include "manager.hpp"

void Manager::create_food_manager(string name)
{
    // TODO create food manager
    int pid = 5; // TODO fork
    food_managers[name] = pid;
    logger->log(CREATE_CHILD, "Created food manager for " + name + " with pid " + to_string(pid));
}

void Manager::create_warehouse_manager(string name)
{
    // TODO create warehouse manager
    int pid = 5; // TODO fork
    warehouse_managers[name] = pid;
    logger->log(CREATE_CHILD, "Created warehouse manager for " + name + " with pid " + to_string(pid));
}

Manager::~Manager()
{
    // TODO close all pipes
}

void Manager::create_food_managers()
{
    csv_reader reader(DATA_DIR + FOOD_FILE);

    CSV data = reader.read();
    logger->log(CSV_READ, "Read " + to_string(data.size()) + " rows from " + FOOD_FILE);

    for (string cell : data[0])
        create_food_manager(cell);
}

void Manager::create_warehouse_managers()
{
    for (const auto &entry : filesystem::directory_iterator(DATA_DIR))
    {
        string filename = entry.path().filename().string();
        if (filename == FOOD_FILE)
            continue;
        filename = entry.path().stem().string();
        create_warehouse_manager(filename);
    }
}
