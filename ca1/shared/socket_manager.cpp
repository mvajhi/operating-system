#include "socket_manager.hpp"

int SocketManager::create_socket(int family, int type, int protocol)
{
    int fd = socket(family, type, protocol);
    handle_socket_creation(fd);
    return fd;
}

void SocketManager::set_socket_options(int socket_fd, int level, int option, int value)
{
    int result = setsockopt(socket_fd, level, option, &value, sizeof(value));
    handle_socket_option(result);
}

void SocketManager::bind_socket(int socket_fd, struct sockaddr_in *addr)
{
    int result = bind(socket_fd, (struct sockaddr *)addr, sizeof(*addr));
    if (result == -1)
        throw runtime_error(ERR_BIND);
}

void SocketManager::listen_socket(int socket_fd, int backlog)
{
    int result = listen(socket_fd, backlog);
    if (result == -1)
        throw runtime_error(ERR_LISTEN);
}

void SocketManager::add_socket(int socket_fd, struct sockaddr_in address)
{
    poll_fds.push_back(pollfd{socket_fd, POLLIN, 0});
    poll_manager->add_descriptor(manager_code, socket_fd, POLLIN);
    socket_map[socket_fd] = address;
}

void SocketManager::check_poll()
{
    int result = poll(poll_fds.data(), poll_fds.size(), -1);
    handle_poll_result(result);
}

bool SocketManager::is_new_massage(pollfd &pfd)
{
    return pfd.revents & POLLIN;
}

pair<int, string> SocketManager::handle_new_message(pollfd &pfd)
{
    if (pfd.fd == STDIN_FILENO)
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        read(STDIN, buffer, BUFFER_SIZE);
        return {STDIN_FILENO, (string)buffer};
    }
    else if (is_server_fd(pfd.fd) && my_type[pfd.fd] == SERVER)
        return {accept_connection(pfd.fd), NEW_CONNECTION_ACCEPTED};
    else
        return process_poll_result(pfd);
}

void SocketManager::handle_invalid_ip(const char *ip, struct sockaddr_in *addr)
{
    if (inet_pton(AF_INET, ip, &addr->sin_addr) == -1)
        throw runtime_error(ERR_INVALID_IP);
}

void SocketManager::try_to_connect(int client_fd, struct sockaddr_in *addr)
{
    if (connect(client_fd, (struct sockaddr *)addr, sizeof(*addr)) == -1)
        throw runtime_error(ERR_CONNECT);
}

void SocketManager::handle_accept_failure(int new_fd)
{
    if (new_fd == -1)
        throw runtime_error(ERR_ACCEPT);
}

void SocketManager::handle_socket_creation(int socket_fd)
{
    if (socket_fd == -1)
        throw runtime_error(ERR_SOCKET_CREATION);
}

void SocketManager::handle_socket_option(int result)
{
    if (result == -1)
        throw runtime_error(ERR_SOCKET_OPTION);
}

void SocketManager::handle_poll_result(int result)
{
    if (result == -1)
        throw runtime_error(ERR_POLL);
}

pair<int, string> SocketManager::process_poll_result(pollfd &pfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(pfd.fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0)
        return {pfd.fd, string(buffer)};
    return NO_NEW_MASSAGE;
}

int SocketManager::setup_socket(const char *ip, int port, sockaddr_in &addr)
{
    addr.sin_family = AF_INET;

    handle_invalid_ip(ip, &addr);
    int socket_fd = create_socket(PF_INET, SOCK_STREAM, 0);
    // int socket_fd = create_socket(PF_INET, SOCK_DGRAM, 0);
    set_socket_options(socket_fd, SOL_SOCKET, SO_REUSEADDR, 1);
    set_socket_options(socket_fd, SOL_SOCKET, SO_REUSEPORT, 1);

    memset(addr.sin_zero, 0, sizeof(addr));
    addr.sin_port = htons(port);

    return socket_fd;
}

SocketManager::SocketManager(int manager_code_, PollManager *poll_manager_)
{
    poll_manager = poll_manager_;
    manager_code = manager_code_;
}

void SocketManager::add_stdin()
{
    poll_fds.push_back(pollfd{STDIN_FILENO, POLLIN, 0});
    poll_manager->add_descriptor(manager_code, STDIN_FILENO, POLLIN);
}

int SocketManager::create_server_socket(const char *ip, int port)
{
    struct sockaddr_in server_addr;
    int server_fd = setup_socket(ip, port, server_addr);

    bind_socket(server_fd, &server_addr);
    listen_socket(server_fd, 20);

    add_socket(server_fd, server_addr);

    my_type[server_fd] = SERVER;
    return server_fd;
}

int SocketManager::create_client_socket(const char *ip, int port)
{
    struct sockaddr_in server_addr;
    int client_fd = setup_socket(ip, port, server_addr);

    try_to_connect(client_fd, &server_addr);

    add_socket(client_fd, server_addr);

    my_type[client_fd] = CLIENT;
    return client_fd;
}

int SocketManager::accept_connection(int server_fd)
{
    struct sockaddr_in new_addr;
    socklen_t new_size = sizeof(new_addr);
    int new_fd = accept(server_fd, (struct sockaddr *)(&new_addr), &new_size);
    handle_accept_failure(new_fd);
    add_socket(new_fd, new_addr);
    my_type[server_fd] = SERVER;
    return new_fd;
}

pair<int, string> SocketManager::receive()
{
    check_poll();
    for (auto &pfd : poll_fds)
        if (is_new_massage(pfd))
            return handle_new_message(pfd);
    return NO_NEW_MASSAGE;
}

void SocketManager::send_message(int socket_fd, const string &message)
{
    if (socket_map.find(socket_fd) != socket_map.end())
        send(socket_fd, message.c_str(), message.length(), 0);
    else
        throw runtime_error(ERR_SOCKET_NOT_FOUND);
}

void SocketManager::close_socket(int socket_fd)
{
    close(socket_fd);
    socket_map.erase(socket_fd);
    poll_fds.erase(remove_if(poll_fds.begin(), poll_fds.end(),
                             [socket_fd](pollfd &pfd)
                             { return pfd.fd == socket_fd; }),
                   poll_fds.end());
}

void SocketManager::close_all_socket()
{
    for (auto &pfd : poll_fds)
        close_socket(pfd.fd);
}

bool SocketManager::is_server_fd(int fd)
{
    return fd == poll_fds[0].fd;
}
