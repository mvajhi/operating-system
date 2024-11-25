#include "csv_reader.hpp"
#include <fstream>
#include <sstream>

using namespace std;

csv_reader::csv_reader(const string& path) : filePath(path) {}

vector<vector<string>> csv_reader::read() {
    vector<vector<string>> data;
    ifstream file(filePath);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            vector<string> row;
            string cell;
            istringstream lineStream(line);

            while (getline(lineStream, cell, ',')) {
                row.push_back(cell);
            }
            data.push_back(row);
        }
        file.close();
    }
    return data;
}
