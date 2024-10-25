#include "define.hpp"
#include "game_manager.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Invalid Arguments\n";
        return 1;
    }

    const char *ipaddr = argv[1];
    int port = strtol(argv[2], NULL, 10);
    int room_count = strtol(argv[3], NULL, 10);

    GameManager game_manager(ipaddr, port, room_count);

    try
    {
        while (true)
        {
            game_manager.handler();
            game_manager.check_routine();
        }
    }
    catch (const runtime_error &e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
