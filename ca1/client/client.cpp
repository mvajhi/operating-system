
#include "define.hpp"
#include "socket_manager.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    const char *ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    SocketManager manager;
    manager.add_stdin();

    try
    {
        int server_fd = manager.create_client_socket(ipaddr, port);

        while (true)
        {
            auto [fd, m] = manager.receive();
            if (fd == STDIN_FILENO)
                manager.send_message(server_fd, m);
            else if (fd != -1)
            {
                cout << "Received from fd " << fd << ": " << m << endl;
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