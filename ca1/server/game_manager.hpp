#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "define.hpp"
#include "room.hpp"
#include "socket_manager.hpp"
#include "poll_manager.hpp"

class GameManager {
public:
    GameManager(const char* ip, int port, int room_count);
    void handler();

private:
    vector<shared_ptr<Player>> players;
    // vector<Room> rooms;
    PollManager poll_manager;
    SocketManager socket_manager;
    int UID;
    int main_ID;

    void main_handler();
    // void end_game();
    bool is_new_fd(int fd);
    void add_player(int fd);
    bool have_name(int fd);
    void set_name(int fd, const string& name);
    void send_rooms_info(int fd);
    // string get_rooms_info();
    bool can_join_room(const string& room_number);
    void join_room(int fd, const string& room_number);

};

#endif 