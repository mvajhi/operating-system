#include "poll_manager.hpp"

void PollManager::add_descriptor(int owner, int fd, short events)
{
    fds.push_back(pollfd{fd, events, 0});
    fds_owner[fds.back().fd] = owner;
}

int PollManager::check_poll()
{
    int result = poll(fds.data(), fds.size(), -1);
    if (result == -1)
        throw runtime_error(ERR_POLL);
    for (auto &pfd : fds)
        if (pfd.revents & POLLIN)
        {
            cout << "PollManager: " << fds_owner[pfd.fd] << " is ready" << endl;
            return fds_owner[pfd.fd];
        }
    return -1;
}
