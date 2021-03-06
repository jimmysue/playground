#include <iostream>

#include "xvision/video/video_frame.h"

using namespace xvision;
using namespace std;

int main(int argc, char** argv){
    
    VideoFrame frame;
    VideoFrame f2(100, 332, xvision::VideoFrame::PixelFormat::AV_PIX_FMT_ARGB);
    auto f3 = std::move(f2);
    cout << "width: " << f2->width << ", " << "height: " << f2->height << ", : " << f2->format << endl;

}