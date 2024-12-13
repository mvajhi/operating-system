#ifndef WAVFILEOPERATIONS_H
#define WAVFILEOPERATIONS_H
#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <pthread.h>
#include <chrono>

void readWavFile(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo);
void writeWavFile(const std::string &outputFile, const std::vector<float> &data, SF_INFO &fileInfo);

void readWavFileParallel(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo);

#endif // WAVFILEOPERATIONS_H