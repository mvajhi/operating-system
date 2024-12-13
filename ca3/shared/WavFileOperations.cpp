#include "WavFileOperations.h"

using namespace std;
void readWavFile(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo)
{
    SNDFILE *inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile)
    {
        std::cerr << "Error opening input file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);
    sf_count_t numFrames = sf_readf_float(inFile, data.data(), fileInfo.frames);
    if (numFrames != fileInfo.frames)
    {
        std::cerr << "Error reading frames from file." << std::endl;
        sf_close(inFile);
        exit(1);
    }

    sf_close(inFile);
    // std::cout << "Successfully read " << numFrames << " frames from " << inputFile << std::endl;
}

void writeWavFile(const std::string &outputFile, const std::vector<float> &data, SF_INFO &fileInfo)
{
    sf_count_t originalFrames = fileInfo.frames;
    SNDFILE *outFile = sf_open(outputFile.c_str(), SFM_WRITE, &fileInfo);
    if (!outFile)
    {
        std::cerr << "Error opening output file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    sf_count_t numFrames = sf_writef_float(outFile, data.data(), originalFrames);
    if (numFrames != originalFrames)
    {
        std::cerr << "Error writing frames to file." << std::endl;
        sf_close(outFile);
        exit(1);
    }

    sf_close(outFile);
    // std::cout << "Successfully wrote " << numFrames << " frames to " << outputFile << std::endl;
}

struct ThreadData {
    std::string inputFile;
    float *data;
    sf_count_t startFrame;
    sf_count_t framesToRead;
    SF_INFO fileInfo;
};

void *readFrames(void *arg) {
    ThreadData *threadData = static_cast<ThreadData *>(arg);
    SNDFILE *inFile = sf_open(threadData->inputFile.c_str(), SFM_READ, &threadData->fileInfo);
    if (!inFile) {
        std::cerr << "Error opening input file in thread: " << sf_strerror(nullptr) << std::endl;
        pthread_exit((void *)1);
    }

    sf_seek(inFile, threadData->startFrame, SEEK_SET);
    sf_count_t readFrames = sf_readf_float(inFile, threadData->data, threadData->framesToRead);
    if (readFrames != threadData->framesToRead) {
        std::cerr << "Error reading frames in thread." << std::endl;
        sf_close(inFile);
        pthread_exit((void *)1);
    }

    sf_close(inFile);
    pthread_exit(nullptr);
}

void readWavFileParallel(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo) {
    SNDFILE *inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile) {
        std::cerr << "Error opening input file: " << sf_strerror(nullptr) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);
    sf_close(inFile); // Close the main file as each thread will open its own

    int numThreads = 4;
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    sf_count_t framesPerThread = fileInfo.frames / numThreads;
    sf_count_t remainderFrames = fileInfo.frames % numThreads;

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].inputFile = inputFile;
        threadData[i].data = data.data() + i * framesPerThread * fileInfo.channels;
        threadData[i].startFrame = i * framesPerThread;
        threadData[i].framesToRead = framesPerThread;
        threadData[i].fileInfo = fileInfo;

        if (i == numThreads - 1) {
            threadData[i].framesToRead += remainderFrames; // Add remaining frames to the last thread
        }

        if (pthread_create(&threads[i], nullptr, readFrames, &threadData[i]) != 0) {
            std::cerr << "Error creating thread." << std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Error joining thread." << std::endl;
            exit(1);
        }
    }
}
