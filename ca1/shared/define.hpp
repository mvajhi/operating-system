#ifndef DEFINE_H
#define DEFINE_H

#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include <map>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/timerfd.h>
// #include <iostream>

using namespace std;

#define STDIN 0
#define STDOUT 1

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

// Define information messages
#define NEW_CONNECTION_ACCEPTED "INFO: New connection accepted"
#define ACCEPTING_CONNECTION "INFO: Accepting new connection\n"

// Define user messages
#define ENTER_NAME_MASSAGE "Enter your name: "
#define SELECT_ROOM_MASSAGE "Select a room: "
#define CANT_JOIN_ROOM_MASSAGE "You can't join this room"

// Define system messages
#define DC_MASSAGE "Disconnecting from sub server...\n"
#define INVALID_ARG "Invalid Arguments\n"
#define CLIENT_LAUNCHED "Client Launched!\n"
#define SERVER_LAUNCHED "Server Launched!\n"

// Define constants
const pair<int, string> NO_NEW_MASSAGE = {-1, ""};

// Define types
#define SERVER 0
#define CLIENT 1
#define BROADCAST 2

// Define move
#define NO_MOVE -1
#define ROCK 0
#define PAPER 1
#define SCISSORS 2

// Define limits
#define GAME_TIME 5

// Define time
#define SLEEP_TIME 1
#define TIMER_POLL_INTERVAL 1

// Define codes
#define DC_CODE "!$!"
#define END_CODE "&%&"
#define LIST_CODE "?"
#define CONNECT_CODE '$'

// Define commands
#define END_GAME_COMMAND "end_game"

// Define server customizations
#define SERVER_BROADCAST_ENABLED false
#define BIP "127.255.255.255"
#define BUFFER_SIZE 1024
#define FIRST_UID 100

typedef struct Player
{
    int fd;
    int sub_fd;
    string name;
    int score;
    bool is_fd_set = false;
    int move = NO_MOVE;
    Player(int fd, int sub_fd, const string &name, int score) : fd(fd), sub_fd(fd), name(name), score(score) {}
} Player;

typedef struct ConnectionDetails
{
    string ip;
    int port;
    string b_ip;
    int b_port;
} ConnectionDetails;

#endif