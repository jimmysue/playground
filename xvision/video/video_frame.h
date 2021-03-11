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

namespace xvision {

class VideoFrame {
  public:
    using PixelFormat = AVPixelFormat;

  public:
    VideoFrame();
    VideoFrame(int width, int height, PixelFormat fmt);
    ~VideoFrame();
    VideoFrame(const VideoFrame &v);
    VideoFrame &operator=(VideoFrame v);  // copy-and-swap
    void swap(VideoFrame &v) noexcept;
    const AVFrame *operator->() const { return _ptr; };
    AVFrame *operator->() { return _ptr; };
    operator AVFrame *() { return _ptr; }
    void create(int width, int height, PixelFormat fmt);
    void release();
    VideoFrame clone() const;
    bool empty() const;
  protected:
    AVFrame *_ptr = nullptr;
};
using PixelFormat = VideoFrame::PixelFormat;

} // namespace xvision
