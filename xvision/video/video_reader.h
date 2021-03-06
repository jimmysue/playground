#pragma once

#include <string>

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

#include "video_frame.h"

namespace xvision {

enum RotationMode { kRotate0, kRotate90, kRotate180, kRotate270 };

class VideoReader {
  public:
    using RotationMode = RotationMode;

  public:
    VideoReader() = default;
    VideoReader(std::string const &filename, int thread_cout = 1);
    ~VideoReader();
    VideoReader(const VideoReader &v);
    VideoReader(VideoReader &&v);
    VideoReader &operator=(const VideoReader &v);
    VideoReader &operator=(VideoReader &&v);

    void open(std::string const &filename, int thread_cout = 1);
    void close();
    bool isOpen() const;

    bool grab();
    VideoFrame retrieve() const;
    VideoFrame retrieveCopy() const;

    // meta info getters
    int width() const;
    int height() const;
    double fps() const;
    int total() const;
    double duration() const;
    RotationMode rotation() const;

  protected:
    void init();

  private:
    int video_stream_idx = -1;
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *video_dec_ctx = nullptr;
    AVStream *video_stream = nullptr;
    VideoFrame frame;
    AVPacket pkt;
};
} // namespace xvision
