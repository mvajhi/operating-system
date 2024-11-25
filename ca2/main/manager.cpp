#include "manager.hpp"

Manager::~Manager()
{
    // TODO close all pipes
}

void Manager::create_food_manager()
{
    csv_reader reader(DATA_DIR + FOOD_FILE);

    CSV data = reader.read();
    logger->log(INFO, "Read " + to_string(data.size()) + " rows from " + FOOD_FILE);

    for (string cell: data[0])
    {
        // TODO create food manager
        logger->log(INFO, "Creating food manager for " + cell);
    }

}

void Manager::create_warehouse_manager()
{
    logger->log(INFO, "Creating warehouse manager");

    for (const auto& entry : filesystem::directory_iterator(DATA_DIR))
    {
        string filename = entry.path().filename().string();
        if (filename == FOOD_FILE)
            continue;
        filename = entry.path().stem().string();
        logger->log(INFO, "Creating warehouse manager for " + filename);
        // TODO create warehouse manager
    }
}
