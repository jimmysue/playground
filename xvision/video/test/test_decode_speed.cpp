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
    auto start = std::chrono::high_resolution_clock::now();
    int count = 0;
    for (; vr.grab() && count < 1000; ++count){
        std::this_thread::sleep_for(10ms);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    double fps = count / std::chrono::duration<double>(finish - start).count();
    std::cout << "fps: " << fps << std::endl;
    return 0;
}