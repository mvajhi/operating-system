#include "game_manager.hpp"

GameManager::GameManager(const char *ip_, int port_, int room_count)
    : socket_manager(FIRST_UID, &poll_manager)
{
    ip = ip_;
    port = port_;
    main_ID = FIRST_UID;
    UID = FIRST_UID + 1;
    broadcast_fd = socket_manager.create_broadcast_socket((const char *)BIP, port);
    socket_manager.create_server_socket(ip, port);
    socket_manager.add_stdin();
    create_rooms(room_count);

    timer_fd = timerfd_create(CLOCK_REALTIME, 0);
    struct itimerspec timer_spec = {};
    timer_spec.it_value.tv_sec = TIMER_POLL_INTERVAL;
    timer_spec.it_interval.tv_sec = TIMER_POLL_INTERVAL;
    timerfd_settime(timer_fd, 0, &timer_spec, NULL);

    UID++;
    time_UID = UID;
    poll_manager.add_descriptor(time_UID, timer_fd, POLLIN);
}

void GameManager::handler()
{
    int result = poll_manager.check_poll();
    if (result == main_ID)
        main_handler();
    for (auto &room : rooms)
        if (result == room.get_UID())
            room.handler();
    if (result == time_UID)
        check_routine();
}

void GameManager::check_routine()
{
    uint64_t expirations;
    read(timer_fd, &expirations, sizeof(expirations));
    for (auto &room : rooms)
        room.check_routine();
}

void GameManager::main_handler()
{
    auto [fd, m] = socket_manager.receive();
     if (fd == STDIN_FILENO)
    {
        if (is_end_game(m))
            end_game();
        else
            send_all(m);
        return;
    }
    else if (fd == -1)
        return;

    cout << "Received from fd " << fd << ": " << m << endl;

    if (m == "?")
        send_rooms_info(fd);
    else if (is_new_fd(fd))
        add_player(fd);
    else if (!have_name(fd))
    {
        m.pop_back();
        set_name(fd, m);
        send_rooms_info(fd);
    }
    else
    {
        if (can_join_room(m))
            join_room(fd, m);
        else
        {
            socket_manager.send_message(fd, CANT_JOIN_ROOM_MASSAGE);
            send_rooms_info(fd);
        }
    }
}

void GameManager::end_game()
{
    send_leader_board();
    close_program();
}

bool GameManager::is_end_game(const string &m)
{
    return m.find(END_GAME_COMMAND) != string::npos;
}

bool GameManager::is_new_fd(int fd)
{
    for (auto &player : players)
        if (player->fd == fd)
            return false;
    return true;
}

void GameManager::add_player(int fd)
{
    players.push_back(make_shared<Player>(fd, -1, "", 0));
    socket_manager.send_message(fd, ENTER_NAME_MASSAGE);
}

bool GameManager::have_name(int fd)
{
    for (auto &player : players)
        if (player->fd == fd)
            return player->name != "";
    return false;
}

void GameManager::set_name(int fd, const string &name)
{
    for (auto &player : players)
        if (player->fd == fd)
            player->name = name;
}

void GameManager::send_rooms_info(int fd)
{
    socket_manager.send_message(fd, get_rooms_info());
    socket_manager.send_message(fd, SELECT_ROOM_MASSAGE);
}

bool GameManager::can_join_room(const string &room_number)
{
    for (auto &room : rooms)
        if (room.get_ID() == stoi(room_number))
        {
            if (room.is_full())
                return false;
            else
                return true;
        }
    return false;
}

void GameManager::join_room(int fd, const string &room_number)
{
    for (auto &player : players)
        if (player->fd == fd)
            rooms[stoi(room_number)].add_player(player);
    for (auto &room : rooms)
        if (room.get_ID() == stoi(room_number))
            socket_manager.send_message(fd, "$" + room.get_connection_info());
}

string GameManager::get_rooms_info()
{
    string info = "";
    for (auto &room : rooms)
    {
        info += "Room " + to_string(room.get_ID()) + " " +
                "have " + to_string(room.get_players().size()) + " players\n";
        info += "\tPlayers:\n";
        for (auto &player : room.get_players())
            info += "\t\t" + player->name + " " + to_string(player->score) + "\n";
    }
    return info;
}

void GameManager::create_rooms(int room_count)
{
    for (int i = 0; i < room_count; i++)
    {
        ++UID;
        rooms.push_back(Room(i, &poll_manager, ip, port + UID, UID, BIP, port + UID+100));
    }
}

void GameManager::send_leader_board()
{
    string massage = "Leader Board\n";
    sort(players.begin(), players.end(), [](shared_ptr<Player> a, shared_ptr<Player> b)
         { return a->score > b->score; });
    int p_size = players.size();
    for (int i = 0; i < p_size; i++)
        massage += to_string(i + 1) + ". " + players[i]->name + " " + to_string(players[i]->score) + "\n";

    send_all(massage);
}

void GameManager::send_all(const string &message)
{
    // for (auto &player : players)
    //     socket_manager.send_message(player->fd, message);
    socket_manager.send_message(broadcast_fd, message);
}

void GameManager::close_program()
{
    send_all(END_CODE);
    socket_manager.close_all_socket();
    for (auto &room : rooms)
        room.close_all_socket();
    exit(0);
}
