#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "define.hpp"
#include "poll_manager.hpp"

class SocketManager {
private:
    vector<pollfd> poll_fds;
    map<int, struct sockaddr_in> socket_map;
    map<int, int> my_type;
    PollManager* poll_manager;
    int manager_code;

    int create_socket(int family, int type, int protocol);
    void set_socket_options(int socket_fd, int level, int option, int value);
    void bind_socket(int socket_fd, struct sockaddr_in* addr);
    void listen_socket(int socket_fd, int backlog);
    void add_socket(int socket_fd, struct sockaddr_in address);
    void check_poll();
    bool is_new_massage(pollfd& pfd);
    bool is_server_fd(int fd);
    pair<int, string> handle_new_message(pollfd& pfd);

    void handle_invalid_ip(const char* ip, struct sockaddr_in* addr);
    void try_to_connect(int client_fd, struct sockaddr_in* addr);
    void handle_accept_failure(int server_fd);
    void handle_socket_creation(int socket_fd);
    void handle_socket_option(int result);
    void handle_poll_result(int result);

    pair<int, string> process_poll_result(pollfd& pfd);

    int setup_socket(const char* ip, int port, sockaddr_in& addr);

public:
    SocketManager(int manager_code_, PollManager* poll_manager_);
    void add_stdin();
    int create_server_socket(const char* ip, int port);
    int create_client_socket(const char* ip, int port);
    int accept_connection(int server_fd);
    pair<int, string> receive();
    void send_message(int socket_fd, const string& message);
    void close_socket(int socket_fd);
};

#endif
