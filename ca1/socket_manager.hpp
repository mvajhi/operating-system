#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include <map>
#include <stdexcept>
#include <string>

using namespace std;

#define BUFFER_SIZE 1024

// Define error messages
#define ERR_SOCKET_CREATION "FAILED: Socket was not created"
#define ERR_SOCKET_OPTION "FAILED: Setting socket option failed"
#define ERR_BIND "FAILED: Bind unsuccessful"
#define ERR_LISTEN "FAILED: Listen unsuccessful"
#define ERR_POLL "FAILED: Poll unsuccessful"
#define ERR_CONNECT "FAILED: Connect unsuccessful"
#define ERR_ACCEPT "FAILED: Accept unsuccessful"
#define ERR_INVALID_IP "FAILED: Invalid IPv4 address"
#define ERR_SOCKET_NOT_FOUND "FAILED: Socket not found"

class SocketManager {
private:
    vector<pollfd> poll_fds;

    int create_socket(int family, int type, int protocol);
    void set_socket_options(int socket_fd, int level, int option, int value);
    void bind_socket(int socket_fd, struct sockaddr_in* addr);
    void listen_socket(int socket_fd, int backlog);
    void add_socket(int socket_fd, struct sockaddr_in address);
    void check_poll();

    void handle_invalid_ip(const char* ip, struct sockaddr_in* addr);
    void handle_connect_failure(int client_fd, struct sockaddr_in* addr);
    void handle_accept_failure(int server_fd);
    void handle_socket_creation(int socket_fd);
    void handle_socket_option(int result);
    void handle_poll_result(int result);

    pair<int, string> process_poll_result(pollfd& pfd);

    int setup_socket(const char* ip, int port, sockaddr_in& addr);

public:
    map<int, struct sockaddr_in> socket_map;
    void create_server_socket(const char* ip, int port);
    void create_client_socket(const char* ip, int port);
    int accept_connection(int server_fd);
    pair<int, string> receive();
    void send_message(int socket_fd, const string& message);
    void close_socket(int socket_fd);
    bool is_server_fd(int fd);
};

#endif
