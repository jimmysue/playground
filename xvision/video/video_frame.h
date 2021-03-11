#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <libyuv/convert.h>
#include <libyuv/rotate.h>
#include <libyuv/scale.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace xvision {

using RotationMode = libyuv::RotationMode;
using PixelFormat = AVPixelFormat;

class VideoFrame {
  public:
    VideoFrame();
    VideoFrame(int width, int height, PixelFormat fmt);
    ~VideoFrame();
    VideoFrame(const VideoFrame &v);
    VideoFrame(VideoFrame &&v);
    VideoFrame &operator=(const VideoFrame &v); // copy-and-swap
    VideoFrame &operator=(VideoFrame &&v);      // copy-and-swap

    void swap(VideoFrame &v) noexcept;
    const AVFrame *operator->() const { return _ptr; };
    AVFrame *operator->() { return _ptr; };
    operator AVFrame *() { return _ptr; }
    void create(int width, int height, PixelFormat fmt);
    void release();
    VideoFrame clone() const;
    bool empty() const;

    // basic image processing

    // TODO: 
    VideoFrame convert(PixelFormat fmt) const;
    VideoFrame scale(int width, int height) const;
    VideoFrame rotate(RotationMode mode) const;

    /// flag = cv::IMREAD_COLOR or cv::IMREAD_GRAYSCALE
    cv::Mat mat(int flag=cv::IMREAD_COLOR) const;

  protected:
    AVFrame *_ptr = nullptr;
};

} // namespace xvision
