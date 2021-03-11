#include <iostream>
#include <sstream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "xvision/video/video_frame.h"
#include "xvision/video/video_reader.h"

using namespace xvision;
using namespace std;

void testVideoReader() {
    VideoReader vr1("/Users/jimmy/Documents/data/videos/n0034aqim33.mp4");
    int count = 0;
    while (vr1.grab()) {
        count++;
        auto frame = vr1.retrieve();
        cout << "frame number: " << vr1.number() << endl;
    }
    cout << "total: " << vr1.total() << " count: " << count << endl;
}

void testVideoSeekKeyFrame() {
    VideoReader vr;
    {
        vr.close();
        vr.grab();
        VideoReader vr2("/Users/jimmy/Documents/data/videos/n0034aqim33.mp4");
        vr2.close();
        bool is_open = vr2.isOpen();
        vr2.open("/Users/jimmy/Documents/data/videos/n0034aqim33.mp4");
        vr = std::move(vr2);
        auto vr3 =
            VideoReader("/Users/jimmy/Documents/data/videos/n0034aqim33.mp4");
    }

    for (int i = 0; i < vr.total(); ++i) {
        int pos = vr.seekKeyFrame(i);
        std::cout << "seek: " << i << " pos: " << pos << std::endl;
    }
    auto rotation = vr.rotation();
    int count = 0;
    int width = vr.width();
    int height = vr.height();
    double fps = vr.fps();
    double duration = vr.duration();
    int k = vr.seekKeyFrame(80.5);
    std::cout << "rotation " << rotation << std::endl
              << "after seek : " << k << std::endl;
}

void testVideoSeekFrame() {
    VideoReader vr("/Users/jimmy/Documents/data/videos/n0034aqim33.mp4", 8);
    int count = 0;
    for (int i = 0; i < vr.total(); ++i) {
        int pos = vr.seekFrame(i);
        std::cout << "seek: " << i << " pos: " << pos << std::endl;
    }
}

void testVideoFrame() {
    VideoFrame frame;
    VideoFrame f2(100, 332, xvision::VideoFrame::PixelFormat::AV_PIX_FMT_ARGB);
    f2 = std::move(f2);
    VideoFrame f3 = f2;
    {
        frame = f3;
        std::swap(frame, f3);
        auto f4 = VideoFrame(100, 332,
                             xvision::VideoFrame::PixelFormat::AV_PIX_FMT_ARGB);
    }

    cout << "width: " << f2->width << ", "
         << "height: " << f2->height << ", : " << f2->format << endl;
    const AVFrame *ptr = f3;
    auto vec = vector<VideoFrame>(3);
    vec.emplace_back(100, 300, PixelFormat::AV_PIX_FMT_ARGB);
    auto v2 = std::move(vec);
}

int main(int argc, char **argv) {
    testVideoFrame();
    testVideoSeekKeyFrame();
    testVideoSeekFrame();
}