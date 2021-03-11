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

#include <libyuv/rotate.h>

#include "video_frame.h"

namespace xvision {

class VideoReader {
  private:
    enum class GrabStatus {
        kNotOpen = 0,
        kReadFrame,
        kSendPacket,
        kReceiveFrame,
        kFlushFrame,
    };

  public:
    VideoReader() { init(); };
    VideoReader(std::string const &filename, int thread_cout = 1);
    ~VideoReader();
    VideoReader(const VideoReader &v) = delete;
    VideoReader(VideoReader &&v);
    VideoReader &operator=(const VideoReader &v) = delete;
    VideoReader &operator=(VideoReader &&v);

    void open(std::string const &filename, int thread_cout = 1);
    void close();
    bool isOpen() const;
    void swap(VideoReader &v) noexcept;
    int seekFrame(int number);
    int seekFrame(double time);
    int seekKeyFrame(int number, bool backward = true);
    int seekKeyFrame(double time, bool backward = true);

    bool grab();
    const VideoFrame &retrieve() const { return frame; };
    VideoFrame retrieveCopy() const { return frame.clone(); };

    // meta info getters
    int width() const { return isOpen() ? video_dec_ctx->width : 0; };
    int height() const { return isOpen() ? video_dec_ctx->height : 0; };
    double fps() const;
    int total() const { return isOpen() ? video_stream->nb_frames : 0; };
    double duration() const;
    RotationMode rotation() const;
    int number() const;

  protected:
    void init();
    int dts2number(int64_t dts) const;
    double dts2sec(int64_t dts) const;
    double r2d(AVRational r) const;

  private:
    GrabStatus status = GrabStatus::kNotOpen;
    int video_stream_idx = -1;
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *video_dec_ctx = nullptr;
    AVStream *video_stream = nullptr;
    VideoFrame frame;
    AVPacket pkt;
};
} // namespace xvision
