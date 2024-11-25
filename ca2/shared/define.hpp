#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>
#include <map>

using namespace std;

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

typedef vector<vector<string>> CSV;

const string DATA_DIR = "stores/";
const string FOOD_FILE = "Parts.csv";

#endif