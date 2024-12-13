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

void Band_pass_filter()
{
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);

    std::vector<float> newAudioData(audioData.size(), 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    auto power2_const = pow(CONST_BPF, 2);
    for (size_t i = 0; i < audioData.size(); i++)
    {
        auto power2 = pow(audioData[i], 2);
        newAudioData[i] = power2 / (power2 + power2_const);
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "BPF time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "Band_pass.wav", newAudioData, fileInfo);
}

float CONST_F0_NOTCH = 0.1;
int N_NOTCH = 2;

void Notch_filter()
{
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);
    std::vector<float> new_audioData(audioData.size(), 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    auto tmp = 2 * N_NOTCH;
    for (size_t i = 0; i < audioData.size(); i++)
    {
        new_audioData[i] = 1 / (1 + pow(audioData[i] / CONST_F0_NOTCH, tmp));
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Notch time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "Notch.wav", new_audioData, fileInfo);
}

int M_FIRF = 10;

void finite_impulse_response_filter()
{
    std::vector<float> h;
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);
    std::vector<float> audioData_new(audioData.size(), 0.0f);

    for (int i = 0; i < M_FIRF; i++)
    {
        h.push_back(1.0 / (M_FIRF / (i + 1)));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < audioData.size(); i++)
    {
        float sum = 0;
        for (int j = 0; j < M_FIRF; j++)
        {
            if (i - j >= 0)
            {
                sum += h[j] * audioData[i - j];
            }
        }
        audioData_new[i] = sum;
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "FIRF time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "FIRF.wav", audioData_new, fileInfo);
}

int M_IIRF = 5;
int N_IIRF = 2;

void infinite_impulse_response_filter()
{
    std::vector<float> a;
    std::vector<float> b;
    SF_INFO fileInfo;
    std::vector<float> audioData;
    std::vector<float> audioData_new;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);

    for (int i = 0; i < M_IIRF; i++)
    {
        b.push_back(1.0 / (M_IIRF / (i + 1)));
    }

    for (int i = 0; i < N_IIRF; i++)
    {
        a.push_back(1.0 / (N_IIRF / (i + 1)));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < (int)audioData.size(); i++)
    {
        float feed_forward = 0;
        float feed_back = 0;
        for (int j = 0; j < M_IIRF; j++)
        {
            if (i - j >= 0)
            {
                feed_forward += b[j] * audioData[i - j];
            }
        }
        for (int j = 1; j <= N_IIRF; j++)
        {
            if (i - j >= 0)
            {
                feed_back += a[j] * audioData_new[i - j];
            }
        }
        audioData_new.push_back(feed_back - feed_forward);
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "IIRF time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(outputFile + "IIRF.wav", audioData_new, fileInfo);
}

int main()
{
    Band_pass_filter();

    Notch_filter();

    finite_impulse_response_filter();

    infinite_impulse_response_filter();

    return 0;
}