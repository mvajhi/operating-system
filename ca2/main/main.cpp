#include "define.hpp"
#include "logger.hpp"
#include "csv_reader.hpp"
#include "manager.hpp"

int main()
{
    Logger logger("main");

    string foods_name;
    cin >> foods_name;
    cout << foods_name << endl;

    Manager manager(&logger);

    manager.create_food_managers(foods_name);
    manager.send_to_warehouse(foods_name);

    logger.log(END, "End of main");
    return 0;
}