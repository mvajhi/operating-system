#include "define.hpp"
#include "socket_manager.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    const char* ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    SocketManager manager;

    try {
        manager.create_server_socket(ipaddr, port);

        while (true) {
            auto [fd, message] = manager.receive();
            if (fd != -1) {
                cout << "Received from fd " << fd << ": " << message << endl;
            }
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
