#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <pthread.h>
#include <chrono>
#include "WavFileOperations.h"

using namespace std;

float CONST_BPF = 1.1;

std::string inputFile = "input.wav";
std::string outputFile = "./out/output";


struct ThreadData {
    const std::vector<float>* audioData;
    std::vector<float>* newAudioData;
    int start;
    int end;
    float power2_const;
};

void* processAudioData(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int i = data->start; i < data->end; ++i) {
        auto power2 = pow((*data->audioData)[i], 2);
        (*data->newAudioData)[i] = power2 / (power2 + data->power2_const);
    }
    return nullptr;
}

void parallelProcessAudioData(const std::vector<float>& audioData, std::vector<float>& newAudioData, int numThreads) {
    std::vector<pthread_t> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);

    float power2_const = pow(CONST_BPF, 2);
    int chunkSize = audioData.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].audioData = &audioData;
        threadData[i].newAudioData = &newAudioData;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? audioData.size() : (i + 1) * chunkSize;
        threadData[i].power2_const = power2_const;

        pthread_create(&threads[i], nullptr, processAudioData, &threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }
}

void Band_pass_filter_parallel()
{
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);

    std::vector<float> newAudioData(audioData.size(), 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    parallelProcessAudioData(audioData, newAudioData, 12);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "BPF parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "Band_pass_parallel.wav", newAudioData, fileInfo);
}
int main()
{
    Band_pass_filter_parallel();
    return 0;
}