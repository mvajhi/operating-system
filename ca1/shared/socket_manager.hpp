#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "define.hpp"
#include "poll_manager.hpp"
#include "print.hpp"

class SocketManager
{
private:
    vector<pollfd> poll_fds;
    map<int, struct sockaddr_in> socket_map;
    map<int, int> my_type;
    PollManager *poll_manager;
    int manager_code;

    // poll handling
    void check_poll();
    pair<int, string> process_poll_result(pollfd &pfd);
    pair<int, string> handle_new_message(pollfd &pfd);

    // Error handling
    void handle_invalid_ip(const char *ip, struct sockaddr_in *addr);
    void try_to_connect(int client_fd, struct sockaddr_in *addr);
    void handle_accept_failure(int server_fd);
    void handle_socket_creation(int socket_fd);
    void handle_socket_option(int result);
    void handle_poll_result(int result);

    // Socket handling
    int create_socket(int family, int type, int protocol);
    int setup_socket(const char *ip, int port, sockaddr_in &addr);
    void set_socket_options(int socket_fd, int level, int option, int value);
    void bind_socket(int socket_fd, struct sockaddr_in *addr);
    void listen_socket(int socket_fd, int backlog);
    void add_socket(int socket_fd, struct sockaddr_in address);

    // Condition checking
    bool is_new_massage(pollfd &pfd);
    bool is_server_fd(int fd);

public:
    SocketManager(int manager_code_, PollManager *poll_manager_);

    // Socket creation methods
    int create_server_socket(const char *ip, int port);
    int create_broadcast_socket(const char *ip, int port, bool poll = false);
    int create_client_socket(const char *ip, int port);
    void add_stdin();

    // Server methods
    int accept_connection(int server_fd);

    // Communication methods
    pair<int, string> receive();
    void send_message(int socket_fd, const string &message);

    // Socket closing methods
    void close_socket(int socket_fd);
    void close_all_socket();
};

#endif
