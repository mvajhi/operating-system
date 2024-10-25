#include "define.hpp"
#include "print.hpp"
#include "socket_manager.hpp"
#include "poll_manager.hpp"

int broadcast_fd = -1;
int room_fd = -1;
int room_broadcast_fd = -1;
int server_fd = -1;
int out_fd = -1;

PollManager poll_manager;
SocketManager manager(FIRST_UID, &poll_manager);

void print_massage(const string &m, int fd)
{
    string prefix = "";
    if (fd == broadcast_fd)
        prefix = "Received from broadcast: ";
    else if (fd == room_broadcast_fd)
        prefix = "Received from room broadcast: ";
    else
        prefix = "Received from fd " + to_string(fd) + ": ";

    print(prefix + m + "\n");
}

ConnectionDetails get_connection_details(const string &massage)
{
    string m = massage;
    string ip = m.substr(1, m.find(' ') - 1);
    m = m.substr(m.find(' ') + 1);
    int port = stoi(m.substr(0, m.find(' ')));
    m = m.substr(m.find(' ') + 1);
    string b_ip = m.substr(0, m.find(' '));
    int b_port = stoi(m.substr(m.find(' ') + 1));

    return {ip, port, b_ip, b_port};
}

void connect_to_subserver(string &m, string &name)
{
    // input: $ip port broadcast_ip broadcast_port
    auto [ip, port, b_ip, b_port] = get_connection_details(m);

    print("Connecting to " + ip + " " + to_string(port) + "\n");
    room_fd = manager.create_client_socket(ip.c_str(), port);
    manager.send_message(room_fd, name);

    print("Connecting to " + b_ip + " " + to_string(b_port) + "\n");
    room_broadcast_fd = manager.create_broadcast_socket(b_ip.c_str(), b_port, true);

    out_fd = room_fd;
}

void dc_from_subserver()
{
    print(DC_MASSAGE);
    manager.close_socket(room_fd);
    out_fd = server_fd;
    manager.send_message(out_fd, LIST_CODE);
}

void handle_massage(string &m, string &name, int &fd)
{
    if (m[0] == CONNECT_CODE)
        connect_to_subserver(m, name);
    else if (m.find(DC_CODE) != string::npos)
        dc_from_subserver();
    else if (m.find(END_CODE) != string::npos)
    {
        manager.close_all_socket();
        exit(0);
    }
    else
        print_massage(m, fd);
}

void handle_stdin(string &m, bool &flag_name, string &name)
{
    manager.send_message(out_fd, m);
    if (flag_name)
    {
        name = m;
        flag_name = false;
    }
}

void connect_to_server(const char *ipaddr, int port)
{
    print("Connecting to " + string(ipaddr) + " " + to_string(port) + "\n");
    server_fd = manager.create_client_socket(ipaddr, port);
    broadcast_fd = manager.create_broadcast_socket(BIP, port, true);
    out_fd = server_fd;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print(INVALID_ARG);
        return 1;
    }

    print(CLIENT_LAUNCHED);

    const char *ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    manager.add_stdin();

    string name;
    bool flag_name = true;
    try
    {
        connect_to_server(ipaddr, port);

        while (true)
        {
            if (poll_manager.check_poll() == -1)
                continue;

            auto [fd, m] = manager.receive();

            if (fd == STDIN_FILENO)
                handle_stdin(m, flag_name, name);
            if (fd <= 1)
                continue;

            // print_massage(m, fd);
            handle_massage(m, name, fd);
        }
    }
    catch (const runtime_error &e)
    {
        string error = e.what();
        write(STDOUT, error.c_str(), error.size());
        return 1;
    }

    return 0;
}
