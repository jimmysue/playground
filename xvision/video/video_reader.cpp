#include "video_reader.h"

#include <exception>

namespace xvision {
VideoReader::VideoReader(std::string const &filename, int thread_cout) {
    open(filename, thread_cout);
}

VideoReader::~VideoReader() {
    if (isOpen()) {
        close();
    }
    init();
}

void VideoReader::open(std::string const &filename, int thread_cout) {
    if (isOpen()) {
        close();
    }
    if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL) < 0) {
        throw std::invalid_argument("avformat_open_input failed");
    }
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    AVMediaType type = AVMediaType::AVMEDIA_TYPE_VIDEO;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, &dec, 0);
    if (ret < 0) {
        throw std::runtime_error("av_find_best_stream failed");
    } else {
        int stream_idx = ret;
        st = fmt_ctx->streams[stream_idx];

        dec_ctx = avcodec_alloc_context3(dec);
        if (!dec_ctx) {
            throw std::runtime_error("avcodec_alloc_context3 failed");
        }

        ret = avcodec_parameters_to_context(dec_ctx, st->codecpar);
        if (ret < 0) {
            throw std::runtime_error("avcodec_parameters_to_context failed");
        }

        dec_ctx->thread_count = thread_cout;
        dec_ctx->thread_type = FF_THREAD_FRAME;

        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
            throw std::runtime_error("avcodec_open2 failed");
        }

        video_stream_idx = stream_idx;
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = dec_ctx;
    }
}

void VideoReader::close() {
    avcodec_free_context(&video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    init();
}

bool VideoReader::isOpen() const {
    return fmt_ctx && video_dec_ctx && video_stream && video_stream_idx >= 0;
}

void VideoReader::swap(VideoReader &v) noexcept {
    std::swap(v.video_stream_idx, video_stream_idx);
    std::swap(v.fmt_ctx, fmt_ctx);
    std::swap(v.video_dec_ctx, video_dec_ctx);
    std::swap(v.frame, frame);
    std::swap(v.pkt, pkt);
    std::swap(v.video_stream, video_stream);
}

void VideoReader::init() {
    video_stream_idx = -1;
    fmt_ctx = nullptr;
    video_dec_ctx = nullptr;
    video_stream = nullptr;
    frame.release();
}

} // namespace xvision
