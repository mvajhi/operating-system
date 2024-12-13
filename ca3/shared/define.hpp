#ifndef DEFINE_H
#define DEFINE_H

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

const string INPUT_FILE = "input.wav";
const string OUTPUT_FILE = "./out/output_";

const float CONST_BPF = 1.1;
const float CONST_F0_NOTCH = 0.1;
const int N_NOTCH = 2;
const int M_FIRF = 10;
const int M_IIRF = 10;
const int N_IIRF = 3;

struct thread_data
{
    const vector<float> *audio_data;
    vector<float> *new_audio_data;
    int start;
    int end;
    vector<float> *h;
    vector<float> *a;
    vector<float> *b;
    float power2_const;
};

#endif