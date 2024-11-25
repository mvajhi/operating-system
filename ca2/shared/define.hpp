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
#include <sys/wait.h>
#include <cstdlib>
#include <memory>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>

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
    CSV_READ,
    RESULT,
    END
};

typedef vector<vector<string>> CSV;

typedef struct Transaction {
    int count;
    double price;
    bool type;
} Transaction;

typedef struct Item {
    int count;
    double profit;
    int remaining_cost;
} Item;

const bool INPUT = true;
const bool OUTPUT = false;

const string DATA_DIR = "stores/";
const string FIFO_DIR = "tmp/";
const string FOOD_FILE = "Parts.csv";
const string WAREHOUSE_PATH = "./warehouse/out/main.out";
const string FOOD_PATH = "./food/out/main.out";

const int CSV_INDEX_NAME = 0;
const int CSV_INDEX_PRICE = 1;
const int CSV_INDEX_COUNT = 2;
const int CSV_INDEX_TYPE = 3;

const string CSV_INPUT = "input\r";
const string CSV_OUTPUT = "output\r";

const int BUFFER_SIZE = 1024;

const char SPLITER = ',';

#endif