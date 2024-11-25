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
    shared_ptr<UnnamedPipe> pipe = make_shared<UnnamedPipe>(logger);
    int pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
        setup_warehouse_child(pipe, name);

    store_warehouse_pipe(pipe, name);

    logger->log(CREATE_CHILD, "Created warehouse manager for " + name + " with pid " + to_string(pid));
}

void Manager::store_warehouse_pipe(shared_ptr<UnnamedPipe> pipe, string &name)
{
    pipe->close_child_write_end();
    pipe->close_child_read_end();
    warehouse_managers[name] = pipe;
}

void Manager::setup_warehouse_child(shared_ptr<UnnamedPipe> pipe, string &name)
{
    pipe->close_parent_write_end();
    pipe->close_parent_read_end();

    char read_fd_str[10], write_fd_str[10];
    snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe->get_parent_to_child_read_fd());
    snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe->get_child_to_parent_write_fd());

    execlp(WAREHOUSE_PATH.c_str(), "warehouse", read_fd_str, write_fd_str, name.c_str(), nullptr);
    perror("execlp failed");
    exit(EXIT_FAILURE);
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

void Manager::send_to_warehouse(string message)
{
    int profit = 0;
    for (const auto &entry : warehouse_managers)
    {
        int tmp = stoi(entry.second->send_and_receive(message));
        profit += tmp;
        logger->log(DEBUG, "Profit for " + entry.first + ": " + to_string(tmp));
    }
    logger->log(OTHER, "Total profit: " + to_string(profit));
}
