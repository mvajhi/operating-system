#include "unnamed_pipe.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

using namespace std;

UnnamedPipe::UnnamedPipe(Logger* _logger) : is_child(false) {
    logger = _logger;
    logger->log(CREATE_PIPE, "Creating unnamed pipe");
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
}

UnnamedPipe::UnnamedPipe(Logger* _logger, int read_fd, int write_fd) : is_child(true) {
    logger = _logger;
    parent_to_child[0] = read_fd;
    child_to_parent[1] = write_fd;
}

UnnamedPipe::~UnnamedPipe() {
    if (is_child) {
        close(parent_to_child[0]);
        close(child_to_parent[1]);
    } else {
        close(parent_to_child[1]);
        close(child_to_parent[0]);
    }
    logger->log(PIPE_CLOSE, "unnamed pipe closed");
}

int UnnamedPipe::get_parent_to_child_read_fd() const {
    return parent_to_child[0];
}

int UnnamedPipe::get_parent_to_child_write_fd() const {
    return parent_to_child[1];
}

int UnnamedPipe::get_child_to_parent_read_fd() const {
    return child_to_parent[0];
}

int UnnamedPipe::get_child_to_parent_write_fd() const {
    return child_to_parent[1];
}

void UnnamedPipe::close_parent_write_end() {
    close(parent_to_child[1]);
}

void UnnamedPipe::close_parent_read_end() {
    close(child_to_parent[0]);
}

void UnnamedPipe::close_child_write_end() {
    close(child_to_parent[1]);
}

void UnnamedPipe::close_child_read_end() {
    close(parent_to_child[0]);
}

void UnnamedPipe::send(const string& message) {
    int write_fd = is_child ? child_to_parent[1] : parent_to_child[1];
    logger->log(PIPE_SEND, "Sending message to unnamed pipe: " + message);
    write(write_fd, message.c_str(), message.size());
}

string UnnamedPipe::receive() {
    logger->log(PIPE_READ, "Reading from unnamed pipe");
    int read_fd = is_child ? parent_to_child[0] : child_to_parent[0];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        return "";
    }
    buffer[bytes_read] = '\0';
    logger->log(PIPE_READ, "Received message from unnamed pipe: " + string(buffer));
    return string(buffer);
}

string UnnamedPipe::send_and_receive(const string& message) {
    send(message);
    return receive();
}