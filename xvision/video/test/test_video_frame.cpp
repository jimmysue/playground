#include <iostream>
#include <vector>

#include "xvision/video/video_frame.h"
#include "xvision/video/video_reader.h"

using namespace xvision;
using namespace std;

void testVideoReader() { VideoReader vr1("/Users/jimmy/Documents/Projects/Github/playground/xvision/asset/test.mp4"); }

void testVideoFrame() {
    VideoFrame frame;
    VideoFrame f2(100, 332, xvision::VideoFrame::PixelFormat::AV_PIX_FMT_ARGB);
    f2 = std::move(f2);
    VideoFrame f3 = f2;
    frame = f3;
    std::swap(frame, f3);
    cout << "width: " << f2->width << ", "
         << "height: " << f2->height << ", : " << f2->format << endl;
    const AVFrame *ptr = f3;
    auto vec = vector<VideoFrame>(3);
    vec.emplace_back(100, 300, PixelFormat::AV_PIX_FMT_ARGB);
    auto v2 = std::move(vec);
}

int main(int argc, char **argv) {
    testVideoFrame();
    testVideoReader();
}