#include "socket_manager.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <cstring>

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 1024

using namespace std;

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
        int server_fd = manager.create_client_socket(ipaddr, port);

        while (true)
        {
            char message[BUFFER_SIZE];
            memset(message, 0, BUFFER_SIZE);
            read(STDIN, message, BUFFER_SIZE);
            manager.send_message(server_fd, (string)message);
        }
    }
    catch (const runtime_error &e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
