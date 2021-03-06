#include <chrono>
#include <iostream>
#include <string>

#include "xvision/video/video_reader.h"

int main(int argc, char **argv) {
    std::cout << argv[1] << std::endl;
    int thread_count = 1;
    if (argc > 2) {
        thread_count = atoi(argv[2]);
    }
    xvision::VideoReader vr(argv[1], thread_count);
    auto start = std::chrono::high_resolution_clock::now();
    int count = 0;
    for (; vr.grab() && count < 10000; ++count)
        ;
    auto finish = std::chrono::high_resolution_clock::now();
    double fps = count / std::chrono::duration<double>(finish - start).count();
    std::cout << "fps: " << fps << std::endl;
    return 0;
}