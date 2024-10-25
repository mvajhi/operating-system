#include "print.hpp"

void print(string m)
{
    write(STDOUT, m.c_str(), m.size());
}