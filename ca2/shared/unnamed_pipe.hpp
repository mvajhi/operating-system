#ifndef unnamed_pipe_hpp
#define unnamed_pipe_hpp

#include "define.hpp"
#include "logger.hpp"

using namespace std;

class UnnamedPipe {
private:
    int parent_to_child[2];
    int child_to_parent[2];
    bool is_child;
    Logger* logger;

public:
    UnnamedPipe(Logger* _logger);
    UnnamedPipe(Logger* _logger, int read_fd, int write_fd);
    ~UnnamedPipe();

    int get_parent_to_child_read_fd() const;
    int get_parent_to_child_write_fd() const;
    int get_child_to_parent_read_fd() const;
    int get_child_to_parent_write_fd() const;

    void close_parent_write_end();
    void close_parent_read_end();
    void close_child_write_end();
    void close_child_read_end();

    void send(const string& message);
    string receive();
    string send_and_receive(const string& message);
};

#endif