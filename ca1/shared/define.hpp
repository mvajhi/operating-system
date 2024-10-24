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
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <memory>

using namespace std;

#define BUFFER_SIZE 1024
#define STDIN 0
#define STDOUT 1

#define FIRST_UID 100

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

// Define user messages
#define ENTER_NAME_MASSAGE "Enter your name: "
#define SELECT_ROOM_MASSAGE "Select a room: "
#define CANT_JOIN_ROOM_MASSAGE "You can't join this room"

// Define constants
const pair<int, string> NO_NEW_MASSAGE = {-1, ""};

#define SERVER 0
#define CLIENT 1

typedef struct Player
{
    int fd;
    string name;
    int score;
    Player(int fd, const string &name, int score) : fd(fd), name(name), score(score) {}
} Player;

#endif