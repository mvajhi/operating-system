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

    try
    {
        manager.create_server_socket(ipaddr, port);
        manager.add_stdin();

        while (true)
        {
            auto [fd, m] = manager.receive();
            if (fd == STDIN_FILENO)
                manager.send_message(4, m);
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
