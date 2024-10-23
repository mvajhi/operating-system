#include "socket_manager.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Invalid Arguments" << endl;
        return 1;
    }

    const char* ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);

    SocketManager manager;

    try {
        manager.create_client_socket(ipaddr, port);

        while (true) {
            string message;
            getline(cin, message);
            manager.send_message(manager.socket_map.begin()->first, message);
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
