#include "game_manager.hpp"

GameManager::GameManager(const char *ip_, int port_, int room_count)
    : socket_manager(FIRST_UID, &poll_manager)
{
    cout << "test game manager" << endl;
    ip = ip_;
    port = port_;
    main_ID = FIRST_UID;
    UID = FIRST_UID + 1;
    socket_manager.create_server_socket(ip, port);
    socket_manager.add_stdin();
    create_rooms(room_count);
}

void GameManager::handler()
{
    if (poll_manager.check_poll() == main_ID)
        main_handler();
    for (auto &room : rooms)
        if (poll_manager.check_poll() == room.get_UID())
            room.handler();
}

void GameManager::main_handler()
{
    auto [fd, m] = socket_manager.receive();
    if (fd == STDIN_FILENO)
        // if (is_end_game(m))
        //     end_game();
        socket_manager.send_message(6, m);
    else if (fd == -1)
        return;

    cout << "Received from fd " << fd << ": " << m << endl;

    if (is_new_fd(fd))
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
        rooms.push_back(Room(i, &poll_manager, ip, port + UID, UID));
    }
}
