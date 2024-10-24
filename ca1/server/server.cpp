#include "define.hpp"
#include "socket_manager.hpp"
#include "poll_manager.hpp"

int main(int argc, char *argv[])
{
    int UID = 100;
    PollManager poll_manager;

    if (argc != 3)
    {
        cout << "Invalid Arguments\n";
        return 1;
    }

    const char *ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    SocketManager manager(UID, &poll_manager);
    int manager_code = UID;
    UID++;
    SocketManager sub_manager(UID, &poll_manager);
    int sub_manager_code = UID;
    UID++;

    try
    {
        manager.create_server_socket(ipaddr, port);
        manager.add_stdin();

        sub_manager.create_server_socket(ipaddr, port + 1);

        while (true)
        {
            if (poll_manager.check_poll() == manager_code)
            {
                auto [fd, m] = manager.receive();
                if (fd == STDIN_FILENO)
                    manager.send_message(6, m);
                else if (fd != -1)
                {
                    cout << "Received from fd " << fd << ": " << m << endl;
                }
            }
            else if (poll_manager.check_poll() == sub_manager_code)
            {
                auto [fd2, m2] = sub_manager.receive();
                if (fd2 != -1)
                {
                    cout << "(sub) Received from fd " << fd2 << ": " << m2 << endl;
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
