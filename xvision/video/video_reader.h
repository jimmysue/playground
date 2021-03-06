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

namespace xvision {

class VideoReader {
  public:
    struct Options;

  public:
    VideoReader();
    VideoReader(std::string const & str, Options opts={});
    ~VideoReader();
    VideoReader(const VideoReader& v);
    VideoReader(VideoReader&& v);
    VideoReader& operator=(const VideoReader& v);
    VideoReader& operator=(VideoReader && v);
    
    void open(std::string const & str, Options opts={});
    void close();
    bool isOpen() const;

};
} // namespace xvision
