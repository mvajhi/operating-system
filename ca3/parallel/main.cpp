#include "define.hpp"

void *process_bp(void *arg)
{
    thread_data *data = static_cast<thread_data *>(arg);
    for (int i = data->start; i < data->end; ++i)
    {
        auto power2 = pow((*data->audio_data)[i], 2);
        (*data->new_audio_data)[i] = power2 / (power2 + data->power2_const);
    }
    return nullptr;
}

void parallel_bp(const vector<float> &audio_data, vector<float> &new_audio_data, int num_threads)
{
    vector<pthread_t> threads(num_threads);
    vector<thread_data> thread_data(num_threads);

    float power2_const = pow(CONST_BPF, 2);
    int chunk_size = audio_data.size() / num_threads;

    for (int i = 0; i < num_threads; ++i)
    {
        thread_data[i].audio_data = &audio_data;
        thread_data[i].new_audio_data = &new_audio_data;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? audio_data.size() : (i + 1) * chunk_size;
        thread_data[i].power2_const = power2_const;

        pthread_create(&threads[i], nullptr, process_bp, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void band_pass_filter_parallel()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    readWavFileParallel(INPUT_FILE, audio_data, file_info);

    vector<float> new_audio_data(audio_data.size(), 0.0f);

    auto start = chrono::high_resolution_clock::now();
    parallel_bp(audio_data, new_audio_data, 12);
    auto end = chrono::high_resolution_clock::now();
    cout << "BPF parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "band_pass_parallel.wav", new_audio_data, file_info);
}

void *process_notch(void *arg)
{
    thread_data *data = static_cast<thread_data *>(arg);
    for (int i = data->start; i < data->end; ++i)
    {
        (*data->new_audio_data)[i] = 1 / (1 + pow((*data->audio_data)[i] / CONST_F0_NOTCH, data->power2_const));
    }
    pthread_exit(nullptr);
}

void parallel_notch(vector<float> &audio_data, vector<float> &new_audio_data, size_t num_threads)
{
    float power2_const = 2 * N_NOTCH;

    pthread_t threads[num_threads];
    thread_data thread_data[num_threads];
    size_t chunk_size = audio_data.size() / num_threads;

    for (size_t i = 0; i < num_threads; ++i)
    {
        thread_data[i].audio_data = &audio_data;
        thread_data[i].new_audio_data = &new_audio_data;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? audio_data.size() : (i + 1) * chunk_size;
        thread_data[i].power2_const = power2_const;
        pthread_create(&threads[i], nullptr, process_notch, &thread_data[i]);
    }

    for (size_t i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void notch_filter_parallel()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    readWavFileParallel(INPUT_FILE, audio_data, file_info);
    vector<float> new_audio_data(audio_data.size(), 0.0f);

    auto start = chrono::high_resolution_clock::now();
    parallel_notch(audio_data, new_audio_data, 12);
    auto end = chrono::high_resolution_clock::now();
    cout << "Notch parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "notch_parallel.wav", new_audio_data, file_info);
}

void *process_fir(void *arg)
{
    thread_data *data = static_cast<thread_data *>(arg);
    for (int i = data->start; i < data->end; ++i)
    {
        float sum = 0;
        for (int j = 0; j < M_FIRF; ++j)
        {
            if (i - j >= 0)
            {
                sum += (*data->h)[j] * (*data->audio_data)[i - j];
            }
        }
        (*data->new_audio_data)[i] = sum;
    }
    pthread_exit(nullptr);
}

void parallel_fir(size_t num_threads, vector<float> &audio_data, vector<float> &new_audio_data, vector<float> &h)
{
    pthread_t threads[num_threads];
    thread_data thread_data[num_threads];
    size_t chunk_size = audio_data.size() / num_threads;

    for (size_t i = 0; i < num_threads; ++i)
    {
        thread_data[i].audio_data = &audio_data;
        thread_data[i].new_audio_data = &new_audio_data;
        thread_data[i].h = &h;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? audio_data.size() : (i + 1) * chunk_size;
        pthread_create(&threads[i], nullptr, process_fir, &thread_data[i]);
    }

    for (size_t i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void finite_impulse_response_filter()
{
    SF_INFO file_info;
    vector<float> audio_data;
    vector<float> h(M_FIRF);

    memset(&file_info, 0, sizeof(file_info));

    readWavFileParallel(INPUT_FILE, audio_data, file_info);
    vector<float> new_audio_data(audio_data.size(), 0.0f);

    for (int i = 0; i < M_FIRF; ++i)
    {
        h[i] = 1.0 / (M_FIRF / (i + 1));
    }

    size_t num_threads = 12;

    auto start = chrono::high_resolution_clock::now();
    parallel_fir(num_threads, audio_data, new_audio_data, h);
    auto end = chrono::high_resolution_clock::now();
    cout << "FIRF parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "firf_parallel.wav", new_audio_data, file_info);
}

void *process_iir(void *arg)
{
    thread_data *data = static_cast<thread_data *>(arg);
    for (int i = data->start; i < data->end; ++i)
    {
        float feed_forward = 0;
        float feed_back = 0;
        for (int j = 0; j < M_IIRF; ++j)
        {
            if (i - j >= 0)
            {
                feed_forward += (*data->b)[j] * (*data->audio_data)[i - j];
            }
        }
        for (int j = 1; j <= N_IIRF; ++j)
        {
            if (i - j >= 0)
            {
                feed_back += (*data->a)[j] * (*data->new_audio_data)[i - j];
            }
        }
        (*data->new_audio_data)[i] = feed_back - feed_forward;
    }
    pthread_exit(nullptr);
}

void parallel_iir(size_t num_threads, vector<float> &audio_data, vector<float> &new_audio_data, vector<float> &a, vector<float> &b)
{
    pthread_t threads[num_threads];
    thread_data thread_data[num_threads];
    size_t chunk_size = audio_data.size() / num_threads;

    for (size_t i = 0; i < num_threads; ++i)
    {
        thread_data[i].audio_data = &audio_data;
        thread_data[i].new_audio_data = &new_audio_data;
        thread_data[i].a = &a;
        thread_data[i].b = &b;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? audio_data.size() : (i + 1) * chunk_size;
        pthread_create(&threads[i], nullptr, process_iir, &thread_data[i]);
    }

    for (size_t i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

void infinite_impulse_response_filter()
{
    SF_INFO file_info;
    vector<float> audio_data;
    vector<float> a(N_IIRF);
    vector<float> b(M_IIRF);

    memset(&file_info, 0, sizeof(file_info));

    readWavFileParallel(INPUT_FILE, audio_data, file_info);
    vector<float> new_audio_data(audio_data.size(), 0.0f);

    for (int i = 0; i < M_IIRF; ++i)
    {
        b[i] = 1.0 / (M_IIRF / (i + 1));
    }

    for (int i = 0; i < N_IIRF; ++i)
    {
        a[i] = 1.0 / (N_IIRF / (i + 1));
    }

    size_t num_threads = 12;

    auto start = chrono::high_resolution_clock::now();
    parallel_iir(num_threads, audio_data, new_audio_data, a, b);
    auto end = chrono::high_resolution_clock::now();
    cout << "IIRF parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    writeWavFile(OUTPUT_FILE + "iirf_parallel.wav", new_audio_data, file_info);
}

void test_read()
{
    SF_INFO file_info;
    vector<float> audio_data;

    memset(&file_info, 0, sizeof(file_info));

    auto start = chrono::high_resolution_clock::now();
    readWavFileParallel(INPUT_FILE, audio_data, file_info);
    auto end = chrono::high_resolution_clock::now();
    cout << "Read parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
}

int main()
{
    auto start = chrono::high_resolution_clock::now();
    test_read();

    band_pass_filter_parallel();

    notch_filter_parallel();

    finite_impulse_response_filter();

    infinite_impulse_response_filter();
    auto end = chrono::high_resolution_clock::now();
    cout << "Total parallel time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    return 0;
}