#ifndef POLL_MANAGER_HPP
#define POLL_MANAGER_HPP

#include "define.hpp"

class PollManager {
public:
    void add_descriptor(int owner, int fd, short events);
    int check_poll();

private:
    vector<struct pollfd> fds;
    map<int, int> fds_owner;
};

#endif