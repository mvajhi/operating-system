#ifndef ROOM_HPP
#define ROOM_HPP

#include "define.hpp"
#include "poll_manager.hpp"
#include "socket_manager.hpp"
#include "print.hpp"

class Room
{
public:
    Room(int room_ID_, PollManager *poll_manager_, const char *ip_, int port_, int UID_,
         const char *b_ip_, int b_port_);

    // Game management methods
    void handler();
    void check_routine();
    void close_all_socket();
    void add_player(shared_ptr<Player> player);

    // Communication methods
    string get_connection_info();
    int get_UID() { return UID; }
    int get_ID() { return room_ID; }
    vector<shared_ptr<Player>> get_players() { return players; }

    bool is_full() { return players.size() == 2; }

private:
    PollManager *poll_manager;
    int room_ID;
    SocketManager socket_manager;
    const char *ip;
    int port;
    const char *b_ip;
    int b_port;
    int broadcast_fd;
    int UID;
    vector<shared_ptr<Player>> players;
    bool is_game_started = false;
    time_t start_time;

    // Game management methods
    void save_start_time();
    void start_game();
    void handle_end_game();
    bool have_winner();
    string get_winner_name();
    void add_score(const string &name);
    void send_win_message(const string &name);
    void send_draw_message();
    void disconnect_players();

    // Message handling methods
    bool is_new_fd(int fd);
    void set_fd(int fd, string m);
    bool is_unset_name(int fd, string m);
    void prepare_game(int fd, string m);
    void game_massage_handler(int fd, string m);

    // Communication methods
    void send_message(int fd, const string &message);
    void send_message_to_all(const string &message);
    void send_game_choice();

    // Time management methods
    bool is_time_over();
};

#endif