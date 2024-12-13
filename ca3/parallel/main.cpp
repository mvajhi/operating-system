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
    std::vector<float>* h;
    std::vector<float>* a;
    std::vector<float>* b;
    float power2_const;
};

void* processAudioData_BP(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int i = data->start; i < data->end; ++i) {
        auto power2 = pow((*data->audioData)[i], 2);
        (*data->newAudioData)[i] = power2 / (power2 + data->power2_const);
    }
    return nullptr;
}

void parallelProcessAudioData_BP(const std::vector<float>& audioData, std::vector<float>& newAudioData, int numThreads) {
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

        pthread_create(&threads[i], nullptr, processAudioData_BP, &threadData[i]);
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

    readWavFileParallel(inputFile, audioData, fileInfo);

    std::vector<float> newAudioData(audioData.size(), 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    parallelProcessAudioData_BP(audioData, newAudioData, 12);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "BPF parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "Band_pass_parallel.wav", newAudioData, fileInfo);
}

const float CONST_F0_NOTCH = 0.1;
const int N_NOTCH = 2;


void* processAudioData_notch(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int i = data->start; i < data->end; ++i) {
        (*data->newAudioData)[i] = 1 / (1 + pow((*data->audioData)[i] / CONST_F0_NOTCH, data->power2_const));
    }
    pthread_exit(nullptr);
}

void parallelProcessAudioData_notch(std::vector<float> &audioData, std::vector<float> &new_audioData, size_t numThreads)
{
    float power2_const = 2 * N_NOTCH;

    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    size_t chunkSize = audioData.size() / numThreads;

    for (size_t i = 0; i < numThreads; ++i)
    {
        threadData[i].audioData = &audioData;
        threadData[i].newAudioData = &new_audioData;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? audioData.size() : (i + 1) * chunkSize;
        threadData[i].power2_const = power2_const;
        pthread_create(&threads[i], nullptr, processAudioData_notch, &threadData[i]);
    }

    for (size_t i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void Notch_filter_parallel() {
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFileParallel(inputFile, audioData, fileInfo);
    std::vector<float> new_audioData(audioData.size(), 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    parallelProcessAudioData_notch(audioData, new_audioData, 12);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Notch parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "Notch_parallel.wav", new_audioData, fileInfo);
}


const int M_FIRF = 10;


void* processAudioData_FIR(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int i = data->start; i < data->end; ++i) {
        float sum = 0;
        for (int j = 0; j < M_FIRF; ++j) {
            if (i - j >= 0) {
                sum += (*data->h)[j] * (*data->audioData)[i - j];
            }
        }
        (*data->newAudioData)[i] = sum;
    }
    pthread_exit(nullptr);
}

void parallelProcessAudioData_FIR(size_t numThreads, std::vector<float> &audioData, std::vector<float> &newAudioData, std::vector<float> &h)
{
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    size_t chunkSize = audioData.size() / numThreads;

    for (size_t i = 0; i < numThreads; ++i)
    {
        threadData[i].audioData = &audioData;
        threadData[i].newAudioData = &newAudioData;
        threadData[i].h = &h;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? audioData.size() : (i + 1) * chunkSize;
        pthread_create(&threads[i], nullptr, processAudioData_FIR, &threadData[i]);
    }

    for (size_t i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void finite_impulse_response_filter() {
    SF_INFO fileInfo;
    std::vector<float> audioData;
    std::vector<float> h(M_FIRF);

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFileParallel(inputFile, audioData, fileInfo);
    std::vector<float> newAudioData(audioData.size(), 0.0f);

    for (int i = 0; i < M_FIRF; ++i) {
        h[i] = 1.0 / (M_FIRF / (i + 1));
    }

    size_t numThreads = 12;

    auto start = std::chrono::high_resolution_clock::now();
    parallelProcessAudioData_FIR(numThreads, audioData, newAudioData, h);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "FIRF parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "FIRF_parallel.wav", newAudioData, fileInfo);
}

const int M_IIRF = 5;
const int N_IIRF = 2;

void* processAudioData(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    for (int i = data->start; i < data->end; ++i) {
        float feed_forward = 0;
        float feed_back = 0;
        for (int j = 0; j < M_IIRF; ++j) {
            if (i - j >= 0) {
                feed_forward += (*data->b)[j] * (*data->audioData)[i - j];
            }
        }
        for (int j = 1; j <= N_IIRF; ++j) {
            if (i - j >= 0) {
                feed_back += (*data->a)[j] * (*data->newAudioData)[i - j];
            }
        }
        (*data->newAudioData)[i] = feed_back - feed_forward;
    }
    pthread_exit(nullptr);
}

void parallelProcessAudioDtaa_IIR(size_t numThreads, std::vector<float> &audioData, std::vector<float> &newAudioData, std::vector<float> &a, std::vector<float> &b)
{
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    size_t chunkSize = audioData.size() / numThreads;

    for (size_t i = 0; i < numThreads; ++i)
    {
        threadData[i].audioData = &audioData;
        threadData[i].newAudioData = &newAudioData;
        threadData[i].a = &a;
        threadData[i].b = &b;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? audioData.size() : (i + 1) * chunkSize;
        pthread_create(&threads[i], nullptr, processAudioData, &threadData[i]);
    }

    for (size_t i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void infinite_impulse_response_filter() {
    SF_INFO fileInfo;
    std::vector<float> audioData;
    std::vector<float> a(N_IIRF);
    std::vector<float> b(M_IIRF);

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFileParallel(inputFile, audioData, fileInfo);
    std::vector<float> newAudioData(audioData.size(), 0.0f);

    for (int i = 0; i < M_IIRF; ++i) {
        b[i] = 1.0 / (M_IIRF / (i + 1));
    }

    for (int i = 0; i < N_IIRF; ++i) {
        a[i] = 1.0 / (N_IIRF / (i + 1));
    }

    size_t numThreads = 12;

    auto start = std::chrono::high_resolution_clock::now();
    parallelProcessAudioDtaa_IIR(numThreads, audioData, newAudioData, a, b);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "IIRF parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "IIRF_parallel.wav", newAudioData, fileInfo);
}

void test_read()
{
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    auto start = std::chrono::high_resolution_clock::now();
    readWavFileParallel(inputFile, audioData, fileInfo);
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Read parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    test_read();

    Band_pass_filter_parallel();

    Notch_filter_parallel();

    finite_impulse_response_filter();

    infinite_impulse_response_filter();
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Total parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    return 0;
}