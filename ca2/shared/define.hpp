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
    OTHER,
    CREATE_PIPE,
    CREATE_CHILD,
    PIPE_READ,
    PIPE_SEND,
    PIPE_CLOSE,
    CSV_READ
};

typedef vector<vector<string>> CSV;

typedef struct Transaction {
    int count;
    int price;
    bool type;
} Transaction;

typedef struct Item {
    int count;
    int profit;
    int remaining_cost;
} Item;

const bool INPUT = true;
const bool OUTPUT = false;

const string DATA_DIR = "stores/";
const string FOOD_FILE = "Parts.csv";

const int CSV_INDEX_NAME = 0;
const int CSV_INDEX_PRICE = 1;
const int CSV_INDEX_COUNT = 2;
const int CSV_INDEX_TYPE = 3;

const string CSV_INPUT = "input\r";
const string CSV_OUTPUT = "output\r";

#endif