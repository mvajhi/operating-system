#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "define.hpp"
#include "room.hpp"
#include "socket_manager.hpp"
#include "poll_manager.hpp"
#include "print.hpp"

class GameManager
{
public:
    GameManager(const char *ip, int port, int room_count);
    void handler();
    void check_routine();

private:
    vector<shared_ptr<Player>> players;
    vector<Room> rooms;
    PollManager poll_manager;
    SocketManager socket_manager;
    int UID;
    int main_ID;
    const char *ip;
    int port;
    int time_UID;
    int timer_fd;
    int broadcast_fd;

    // Game management methods
    void main_handler();
    void handle_stdin(string &m);
    void handle_massage_main(string &m, int &fd);
    void handle_normal_massage(string &m, int &fd);

    // Setup methods
    void setup_connection();
    void setup_timer();
    void create_rooms(int room_count);

    // New player methods
    void add_player(int fd);
    void set_name(int fd, const string &name);

    // end game methods
    void end_game();
    void close_program();

    // Connection P2R methods
    void join_room(int fd, const string &room_number);
    void pass_room_to_player(const string &room_number, int fd);
    void pass_player_to_room(int fd, const string &room_number);

    // Send methods
    void send_rooms_info(int fd);
    void send_leader_board();
    void send_all(const string &message);

    // Get Room info methods
    string get_rooms_info();
    string get_room_info(Room &room);

    // Condition methods
    bool is_end_game(const string &m);
    bool is_new_fd(int fd);
    bool is_all_digit(const string &s);
    bool can_join_room(const string &room_number);
    bool have_name(int fd);
};

#endif