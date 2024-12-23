#include "room.hpp"

Room::Room(int room_ID_, PollManager *poll_manager_, const char *ip_,
           int port_, int UID_, const char *b_ip_, int b_port_)
    : poll_manager(poll_manager_), room_ID(room_ID_), socket_manager(UID_, poll_manager_)
{
    socket_manager.create_server_socket(ip_, port_);
    UID = UID_;
    ip = ip_;
    port = port_;

    broadcast_fd = socket_manager.create_broadcast_socket(b_ip_, b_port_, true);
    b_ip = b_ip_;
    b_port = b_port_;
}

void Room::add_player(shared_ptr<Player> player)
{
    players.push_back(player);
}

void Room::handler()
{
    if (poll_manager->check_poll() == UID)
    {
        auto [fd, m] = socket_manager.receive();
        print("(sub " + to_string(room_ID) + ")Received from fd " + to_string(fd) + ": " + m + "\n");
        if (fd == broadcast_fd)
            return;
        if (!is_game_started)
            prepare_game(fd, m);
        else
            game_massage_handler(fd, m);
    }
}

bool Room::is_new_fd(int fd)
{
    for (auto &player : players)
        if (player->sub_fd == fd)
            return false;
    return true;
}

void Room::set_fd(int fd, string m)
{
    for (auto &player : players)
        if (player->name == m)
            player->sub_fd = fd;
}

bool Room::is_unset_name(int fd, string m)
{
    for (auto &player : players)
        if (player->sub_fd == fd && player->name == m)
            return true;
    return false;
}

void Room::prepare_game(int fd, string m)
{
    if (m == "" && is_new_fd(fd))
    {
        print("(sub " + to_string(room_ID) + ")New player: " + to_string(fd) + m + "\n");
        return;
    }
    else if (m != "" && (is_new_fd(fd) || is_unset_name(fd, m)))
    {
        if (m.find("INFO") == 0)
            return;
        if (m.back() == '\n')
            m.pop_back();

        print("(sub " + to_string(room_ID) + ")Player " + m + " is ready\n");
        set_fd(fd, m);
        send_message(fd, "Welcome " + m);

        if (players.size() == 2)
            start_game();
    }
}

void Room::game_massage_handler(int fd, string m)
{
    if (is_time_over())
        return;
    for (auto &player : players)
        if (player->sub_fd == fd)
        {
            if (player->move != NO_MOVE)
                return;
            m.pop_back();
            int move = stoi(m);
            if (move != ROCK && move != PAPER && move != SCISSORS)
                return;
            player->move = move;
            string move_name = (move == ROCK) ? "Rock" : (move == PAPER) ? "Paper"
                                                                         : "Scissors";
            send_message(fd, "You chose " + move_name);
        }
}

void Room::send_message(int fd, const string &message)
{
    socket_manager.send_message(fd, message);
}

void Room::send_message_to_all(const string &message)
{
    // for (auto &player : players)
    //     socket_manager.send_message(player->sub_fd, message);
    socket_manager.send_message(broadcast_fd, message);
}

void Room::send_game_choice()
{
    string message = "\nChoose your move: ";
    message += to_string(ROCK) + " Rock\n";
    message += "\t\t  " + to_string(PAPER) + " Paper\n";
    message += "\t\t  " + to_string(SCISSORS) + " Scissors\n";
    send_message_to_all(message);
}

string Room::get_connection_info()
{
    return string(ip) + " " + to_string(port) + " " +
           string(b_ip) + " " + to_string(b_port);
}

void Room::check_routine()
{
    if (is_game_started && is_time_over())
        handle_end_game();
}

bool Room::is_time_over()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec - start_time >= GAME_TIME;
}

void Room::close_all_socket()
{
    socket_manager.close_all_socket();
}

void Room::save_start_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    start_time = ts.tv_sec;
}

void Room::start_game()
{
    is_game_started = true;
    players[0]->move = NO_MOVE;
    players[1]->move = NO_MOVE;
    send_message_to_all("Game started\n");
    send_game_choice();
    save_start_time();
}

void Room::handle_end_game()
{
    is_game_started = false;
    send_message_to_all("Game ended\n");
    if (have_winner())
    {
        string winner_name = get_winner_name();
        add_score(winner_name);
        send_win_message(winner_name);
    }
    else
    {
        send_draw_message();
    }
    sleep(SLEEP_TIME);
    disconnect_players();
}

bool Room::have_winner()
{
    return players[0]->move != players[1]->move;
}

string Room::get_winner_name()
{
    int p1_choice = players[0]->move;
    string p1_name = players[0]->name;
    int p2_choice = players[1]->move;
    string p2_name = players[1]->name;

    if (p1_choice == ROCK && p2_choice == SCISSORS)
        return p1_name;
    if (p1_choice == PAPER && p2_choice == ROCK)
        return p1_name;
    if (p1_choice == SCISSORS && p2_choice == PAPER)
        return p1_name;
    return p2_name;
}

void Room::add_score(const string &name)
{
    for (auto &player : players)
        if (player->name == name)
            player->score++;
}

void Room::send_win_message(const string &name)
{
    send_message_to_all(name + " won the game\n");
    for (auto &player : players)
        if (player->name == name)
            send_message(player->sub_fd, "You won the game\n");
        else
            send_message(player->sub_fd, "You lost the game\n");
}

void Room::send_draw_message()
{
    send_message_to_all("Draw\n");
}

void Room::disconnect_players()
{
    // send ! to player to disconnect
    send_message_to_all(DC_CODE);
    sleep(SLEEP_TIME);
    // close socket
    for (auto &player : players)
        socket_manager.close_socket(player->sub_fd);
    // clear sub_fd and move
    for (auto &player : players)
    {
        player->sub_fd = -1;
        player->move = NO_MOVE;
    }
    // clear vector players
    players.clear();
}
