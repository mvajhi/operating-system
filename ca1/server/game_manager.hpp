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
    void check_routine();

private:
    vector<shared_ptr<Player>> players;
    vector<Room> rooms;
    PollManager poll_manager;
    SocketManager socket_manager;
    int UID;
    int main_ID;
    const char* ip;
    int port;
    int time_UID;
    int timer_fd;
    int broadcast_fd;

    void main_handler();
    void end_game();
    bool is_end_game(const string& m);
    bool is_new_fd(int fd);
    void add_player(int fd);
    bool have_name(int fd);
    void set_name(int fd, const string& name);
    void send_rooms_info(int fd);
    bool can_join_room(const string& room_number);
    void join_room(int fd, const string& room_number);
    string get_rooms_info();
    void create_rooms(int room_count);
    void send_leader_board();
    void send_all(const string& message);
    void close_program();

};

#endif 