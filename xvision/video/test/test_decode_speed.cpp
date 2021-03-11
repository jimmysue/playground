#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "xvision/video/video_reader.h"

using namespace std::chrono_literals;

int main(int argc, char **argv) {
    std::cout << argv[1] << std::endl;
    int thread_count = 1;
    if (argc > 2) {
        thread_count = atoi(argv[2]);
    }
    xvision::VideoReader vr(argv[1], thread_count);

    cv::Mat frame;
    vr.grab();

    {
        auto start = std::chrono::high_resolution_clock::now();
        for (auto i = 0; i < 1000; ++i) {
            frame = vr.retrieve().mat();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        double fps =
            1000 / std::chrono::duration<double>(finish - start).count();
        std::cout << "convert-fps: " << fps << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();

    int count = 0;
    for (; vr.grab() && count < 1000; ++count) {
        frame = vr.retrieve().mat();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    double fps = count / std::chrono::duration<double>(finish - start).count();
    std::cout << "fps: " << fps << std::endl;
    return 0;
}