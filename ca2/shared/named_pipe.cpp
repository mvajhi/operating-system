#include "named_pipe.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

NamedPipe::NamedPipe(Logger *_logger, const string &name)
    : fifo_name(name), fd(-1), logger(_logger) {}

NamedPipe::~NamedPipe() {
    // close_pipe();
}

bool NamedPipe::create() {
    logger->log(CREATE_PIPE, "Creating FIFO pipe " + fifo_name);
    if (mkfifo(fifo_name.c_str(), 0666) == -1) {
        logger->log(CREATE_PIPE, "Failed to create FIFO pipe " + fifo_name);
        return false;
    }
    return true;
}

bool NamedPipe::open_for_reading() {
    logger->log(CREATE_PIPE, "Opening FIFO pipe " + fifo_name + " for reading");
    fd = open(fifo_name.c_str(), O_RDONLY);
    if (fd == -1) {
        logger->log(DEBUG, "Failed to open FIFO for reading");
        return false;
    }
    return true;
}

bool NamedPipe::open_for_writing() {
    logger->log(CREATE_PIPE, "Opening FIFO pipe " + fifo_name + " for writing");
    fd = open(fifo_name.c_str(), O_WRONLY);
    if (fd == -1) {
        logger->log(DEBUG, "Failed to open FIFO for writing");
        return false;
    }
    return true;
}

void NamedPipe::write_to_pipe(const string& message) {
    if (fd == -1) {
        logger->log(DEBUG, "Pipe is not open for writing!");
        return;
    }
    write(fd, message.c_str(), message.size() + 1);
    logger->log(PIPE_SEND, "Sent message to pipe: " + message);
}

string NamedPipe::read_from_pipe() {
    if (fd == -1) {
        logger->log(DEBUG, "Pipe is not open for reading!");
        return "";
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        logger->log(DEBUG, "Failed to read from pipe");
        return "";
    }

    buffer[bytes_read] = '\0';
    logger->log(PIPE_READ, "Read message from pipe: " + string(buffer));
    return string(buffer);
}

void NamedPipe::close_pipe() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
    logger->log(PIPE_CLOSE, "Closed pipe");
}

void NamedPipe::remove_pipe() {
    unlink(fifo_name.c_str());
}