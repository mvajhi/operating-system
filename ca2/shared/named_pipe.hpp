#ifndef named_pipe_hpp
#define named_pipe_hpp

#include "define.hpp"
#include "logger.hpp"

class NamedPipe
{
private:
    string fifo_name;
    int fd;
    Logger *logger;

public:
    NamedPipe(Logger *_logger, const string &name);
    ~NamedPipe();

    bool create();
    bool open_for_reading();
    bool open_for_writing();
    void write_to_pipe(const string &message);
    string read_from_pipe();
    void close_pipe();
    void remove_pipe();
};

#endif