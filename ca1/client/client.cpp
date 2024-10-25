
#include "define.hpp"
#include "socket_manager.hpp"
#include "poll_manager.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    const char *ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    PollManager poll_manager;

    SocketManager manager(106, &poll_manager);
    manager.add_stdin();

    try
    {
        int server_fd = manager.create_client_socket(ipaddr, port);

        int broadcast_fd = -1;
        broadcast_fd = manager.create_broadcast_socket(BIP, port, true);
        int room_fd = -1;
        int room_broadcast_fd = -1;
        bool flag_name = true;
        auto out_fd = server_fd;
        string name;

        while (true)
        {
            if (poll_manager.check_poll() != -1)
            {
                auto [fd, m] = manager.receive();
                if (fd == STDIN_FILENO)
                {
                    manager.send_message(out_fd, m);
                    if (flag_name)
                    {
                        name = m;
                        flag_name = false;
                    }
                }
                else if (fd != -1)
                {
                    if (fd == broadcast_fd)
                    {
                        cout << "Received from broadcast: " << m << endl;
                    }
                    else if (fd == room_broadcast_fd)
                    {
                        cout << "Received from room broadcast: " << m << endl;
                    }
                    else
                        cout << "Received from fd " << fd << ": " << m << endl;
                    
                    if (m[0] == '$')
                    {
                        // input: $ip port broadcast_ip broadcast_port
                        string ip = m.substr(1, m.find(' ') - 1);
                        m = m.substr(m.find(' ') + 1);
                        int port = stoi(m.substr(0, m.find(' ')));
                        m = m.substr(m.find(' ') + 1);
                        string b_ip = m.substr(0, m.find(' '));
                        int b_port = stoi(m.substr(m.find(' ') + 1));

                        cout << "Connecting to " << ip << " " << port << endl;
                        room_fd = manager.create_client_socket(ip.c_str(), port);
                        manager.send_message(room_fd, name);

                        cout << "Connecting to " << b_ip << " " << b_port << endl;
                        room_broadcast_fd = manager.create_broadcast_socket(b_ip.c_str(), b_port, true);

                        out_fd = room_fd;
                    }
                    else if (m.find(DC_CODE) != string::npos)
                    {
                        cout << "Disconnected from sub server" << endl;
                        manager.close_socket(room_fd);
                        out_fd = server_fd;
                        manager.send_message(out_fd, "?");
                    }
                    else if (m.find(END_CODE) != string::npos)
                    {
                        manager.close_all_socket();
                        exit(0);
                    }
                }
            }
        }
    }
    catch (const runtime_error &e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}