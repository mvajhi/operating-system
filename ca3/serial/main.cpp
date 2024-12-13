#include "define.hpp"

void band_pass_filter()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    readWavFile(INPUT_FILE, audio_data, file_info);

    vector<float> new_audio_data(audio_data.size(), 0.0f);

    auto start = chrono::high_resolution_clock::now();
    auto power2_const = pow(CONST_BPF, 2);
    for (size_t i = 0; i < audio_data.size(); i++)
    {
        auto power2 = pow(audio_data[i], 2);
        new_audio_data[i] = power2 / (power2 + power2_const);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "BPF time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "band_pass.wav", new_audio_data, file_info);
}

void notch_filter()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    readWavFile(INPUT_FILE, audio_data, file_info);
    vector<float> new_audio_data(audio_data.size(), 0.0f);

    auto start = chrono::high_resolution_clock::now();
    auto tmp = 2 * N_NOTCH;
    for (size_t i = 0; i < audio_data.size(); i++)
    {
        new_audio_data[i] = 1 / (1 + pow(audio_data[i] / CONST_F0_NOTCH, tmp));
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "Notch time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "notch.wav", new_audio_data, file_info);
}

void finite_impulse_response_filter()
{
    vector<float> h;
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    readWavFile(INPUT_FILE, audio_data, file_info);
    vector<float> audio_data_new(audio_data.size(), 0.0f);

    for (int i = 0; i < M_FIRF; i++)
    {
        h.push_back(1.0 / (M_FIRF / (i + 1)));
    }

    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < audio_data.size(); i++)
    {
        float sum = 0;
        for (int j = 0; j < M_FIRF; j++)
        {
            if (i - j >= 0)
            {
                sum += h[j] * audio_data[i - j];
            }
        }
        audio_data_new[i] = sum;
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "FIRF time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "firf.wav", audio_data_new, file_info);
}

void infinite_impulse_response_filter()
{
    vector<float> a;
    vector<float> b;
    SF_INFO file_info;
    vector<float> audio_data;
    vector<float> audio_data_new;

    memset(&file_info, 0, sizeof(file_info));

    readWavFile(INPUT_FILE, audio_data, file_info);

    for (int i = 0; i < M_IIRF; i++)
    {
        b.push_back(1.0 / (M_IIRF / (i + 1)));
    }

    for (int i = 0; i < N_IIRF; i++)
    {
        a.push_back(1.0 / (N_IIRF / (i + 1)));
    }

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < (int)audio_data.size(); i++)
    {
        float feed_forward = 0;
        float feed_back = 0;
        for (int j = 0; j < M_IIRF; j++)
        {
            if (i - j >= 0)
            {
                feed_forward += b[j] * audio_data[i - j];
            }
        }
        for (int j = 1; j <= N_IIRF; j++)
        {
            if (i - j >= 0)
            {
                feed_back += a[j] * audio_data_new[i - j];
            }
        }
        audio_data_new.push_back(feed_back - feed_forward);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "IIRF time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "iirf.wav", audio_data_new, file_info);
}

void test_read()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    auto start = chrono::high_resolution_clock::now();
    readWavFile(INPUT_FILE, audio_data, file_info);
    auto end = chrono::high_resolution_clock::now();
    cout << "Read time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
}

int main()
{
    auto start = chrono::high_resolution_clock::now();
    test_read();

    band_pass_filter();

    notch_filter();

    finite_impulse_response_filter();

    infinite_impulse_response_filter();
    auto end = chrono::high_resolution_clock::now();
    cout << "Total time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    return 0;
}