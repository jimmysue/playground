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
    VideoFrame f2(100, 332, xvision::PixelFormat::AV_PIX_FMT_ARGB);
    f2 = std::move(f2);
    VideoFrame f3 = f2;
    {
        frame = f3;
        std::swap(frame, f3);
        auto f4 = VideoFrame(100, 332, xvision::PixelFormat::AV_PIX_FMT_ARGB);
    }

    cout << "width: " << f2->width << ", "
         << "height: " << f2->height << ", : " << f2->format << endl;
    const AVFrame *ptr = f3;
    auto vec = vector<VideoFrame>(3);
    vec.emplace_back(100, 300, PixelFormat::AV_PIX_FMT_ARGB);
    auto v2 = std::move(vec);
}

void testVideoFrameProc() {
    VideoReader vr1("http://9.21.8.131:80/vhqts.tc.qq.com/"
                    "PdRknhvF5FuPOq0kis2acm-8U-3-"
                    "Px8baSPkuaHpyliR7r1ZFvqVMSv3IUkrSLYSVSYyKRljatMQDeFWXTwa6_"
                    "AAf3Vlzrxl9JFgUjVYnuSob_WTUBBdnQ/"
                    "d00361djxcz.322019.1.mp4?ver=5&sha="
                    "6a1f0cb469728d5301fc1cf565650fa058ed2e8b");
    int count = 0;
    vr1.seekFrame(15.0);
    std::cout << "duration: " << vr1.duration() << std::endl;
    std::cout << "total: " << vr1.total() << std::endl;
    cout << vr1.fps();
    while (vr1.grab()) {
        count++;
        auto frame = vr1.retrieve();
        std::stringstream ss;

        ss << count << ".jpeg";
        cout << vr1.number() << endl;
        cv::imwrite(ss.str(), frame.mat());

        ss.clear();
        ss.str("");
        ss << count << "-gray.jpeg";
        cv::imwrite(ss.str(), frame.mat(cv::IMREAD_GRAYSCALE));

        auto scaled = frame.scale(100, 100);
        ss.clear();
        ss.str("");
        ss << count << "-scale.jpeg";
        cv::imwrite(ss.str(), scaled.mat());
    }
    cout << "total: " << vr1.total() << " count: " << count << endl;
}

void testVideoMeta() {
    std::string filename =
        "http://100.94.12.42:80/vfilets.tc.qq.com/"
        "Z3sKB6sSaWV4KAy105ErdRec-waZ-MWdyZM5NhymbKzlU9Cdgk-"
        "JNtrWkd0zsLlDx1usJGie5GtpWp0NMZizHu1MZOJuh4jo6sNIvgyxBOdOM0DK-nUxdA/"
        "c0025973n2t.321002.ts.m3u8?ver=4&sha="
        "2791eb2bd4bae1eed828abe5241cd294d583b66d";
    VideoReader vr(filename);
    cout << "file duration:" << vr.duration() << endl
         << "total: " << vr.total() << endl
         << "fps: " << vr.fps() << endl;
    int k = vr.seekFrame(600.0);
    for (auto i = 0; i < 10 && vr.grab(); ++i) {
        auto image = vr.retrieve().mat();
        auto timestamp = vr.timestamp();
        auto ts = double(vr.number()) / vr.total() * vr.duration();
        std::cout << "timestamp: " << timestamp << std::endl;
        std::string filename =
            std::to_string(i) + "-" + std::to_string(vr.number()) + ".jpg";
        cv::imwrite(filename, image);
    }
}

int main(int argc, char **argv) {
    //    testVideoFrameProc();
    testVideoMeta();
    //    testVideoSeekKeyFrame();
    //    testVideoSeekFrame();
}