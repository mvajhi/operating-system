#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include "define.hpp"

class csv_reader {
private:
    string filePath;

public:
    csv_reader(const string& path);
    vector<vector<string>> read();
};

#endif
