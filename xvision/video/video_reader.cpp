#include "video_reader.h"

#include <exception>
#include <iostream>

namespace xvision {

namespace detail {
class FFmpegRegister {
  public:
    static void init() { static FFmpegRegister instance; }

  private:
    FFmpegRegister() {
        avcodec_register_all();
        av_register_all();
    }
};
} // namespace detail

VideoReader::VideoReader(std::string const &filename, int thread_cout)
    : VideoReader() {
    open(filename, thread_cout);
}

VideoReader::VideoReader(VideoReader &&v) : VideoReader() { this->swap(v); }

VideoReader &VideoReader::operator=(VideoReader &&v) {
    if (this != &v) {
        close();
        swap(v);
    }
    return *this;
}

VideoReader::~VideoReader() {
    if (isOpen()) {
        close();
    }
    init();
}

void VideoReader::open(std::string const &filename, int thread_cout) {
    detail::FFmpegRegister::init();
    close();
    if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL) < 0) {
        throw std::invalid_argument("avformat_open_input failed");
    }
    int ret;
    AVStream *st = nullptr;
    AVCodecContext *dec_ctx = nullptr;
    AVCodec *dec = nullptr;
    AVDictionary *opts = nullptr;
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
    if (video_dec_ctx) {
        avcodec_free_context(&video_dec_ctx);
    }
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }
    if (pkt.data) {
        av_packet_unref(&pkt);
    }
    frame.release();
    init();
}

bool VideoReader::isOpen() const {
    return fmt_ctx && video_dec_ctx && video_stream && video_stream_idx >= 0;
}

void VideoReader::swap(VideoReader &v) noexcept {
    std::swap(status, v.status);
    std::swap(fmt_ctx, v.fmt_ctx);
    std::swap(video_dec_ctx, v.video_dec_ctx);
    std::swap(video_stream_idx, v.video_stream_idx);
    std::swap(video_stream, v.video_stream);
    std::swap(frame, v.frame);
    std::swap(pkt, v.pkt);
}

bool VideoReader::grab() {
    int ret = 0;
    switch (status) {
    case GrabStatus::kReadFrame:
        goto read_frame;
    case GrabStatus::kSendPacket:
        goto send_packet;
    case GrabStatus::kReceiveFrame:
        goto receive_frame;
    case GrabStatus::kFlushFrame:
        goto flush_frame;
    default:
        return false;
    }
read_frame:
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_idx) {
        send_packet:
            ret = avcodec_send_packet(video_dec_ctx, &pkt);

            while (ret >= 0) {
                ret = avcodec_receive_frame(video_dec_ctx, frame);
                if (ret >= 0) {
                    status = GrabStatus::kReceiveFrame;
                    return true;
                } else if (ret == AVERROR(EAGAIN)) {
                    break;
                } else if (ret == AVERROR_EOF) {
                    return false;
                } else if (ret < 0) {
                    throw std::runtime_error("avcodec_receive_frame error");
                }
            receive_frame:;
            }
        }
        av_packet_unref(&pkt);
    }
    // flush
    ret = avcodec_send_packet(video_dec_ctx, nullptr);
    while (ret >= 0) {
        ret = avcodec_receive_frame(video_dec_ctx, frame);
        if (ret == AVERROR(EAGAIN)) {
            break;
        } else if (ret == AVERROR_EOF) {
            return false;
        } else if (ret < 0) {
            throw std::runtime_error("avcodec_receive_frame error");
        }
        status = GrabStatus::kFlushFrame;
        return true;
    flush_frame:;
    }
    return false;
}

double VideoReader::fps() const {
    double fps = r2d(video_stream->avg_frame_rate);
    if (fps < 1e-9) {
        fps = 1.0 / r2d(video_dec_ctx->time_base);
    }
    return fps;
}

double VideoReader::duration() const {
    double sec = (double)video_stream->duration * r2d(video_stream->time_base);
    if (sec < 1e-9) {
        sec = (double)(video_stream->duration) / double(AV_TIME_BASE);
    }
    return sec;
}

RotationMode VideoReader::rotation() const {
    int rotate = 0;
    AVDictionaryEntry *rotate_tag =
        av_dict_get(video_stream->metadata, "rotate", NULL, 0);
    if (rotate_tag != NULL)
        rotate = atoi(rotate_tag->value);
    int index = rotate / 90 % 4;
    index = index < 0 ? 0 : index;
    static const RotationMode modes[4] = {
        RotationMode::kRotate0, RotationMode::kRotate90,
        RotationMode::kRotate180, RotationMode::kRotate270};
    return modes[index];
}

int VideoReader::number() const {
    if (isOpen()) {
        return dts2number(frame->best_effort_timestamp -
                          video_stream->start_time);
    }
    return -1;
}

int VideoReader::seekFrame(int number) {
    number = std::max(0, std::min(total(), number));
    int delta = 2;
    int pos = seekKeyFrame(number - delta);
    while (pos > number) {
        delta = delta < 16 ? delta * 2 : delta * 3 / 2;
        pos = seekKeyFrame(number - delta);
    }
    while (pos < number) {
        grab();
        pos = this->number() + 1;
    }
    return pos;
}

int VideoReader::seekFrame(double time) {
    return seekFrame(int(time * fps() + 0.5));
}

int VideoReader::seekKeyFrame(int number, bool backward) {
    if (pkt.data) {
        av_packet_unref(&pkt);
    }
    number = std::max(0, std::min(total(), number));
    double second = number / fps();
    double timebase = r2d(video_stream->time_base);
    int64_t timestamp =
        video_stream->start_time + (int64_t)(second / timebase + .5);
    if (total() > 0) {
        int flag = backward ? AVSEEK_FLAG_BACKWARD : 0;
        int ret = av_seek_frame(fmt_ctx, video_stream_idx, timestamp, flag);
        if (ret < 0) {
            throw std::runtime_error("av_seek_frame failed");
        }
        avcodec_flush_buffers(video_dec_ctx);
        while (av_read_frame(fmt_ctx, &pkt) >= 0) {

            if (pkt.stream_index == video_stream_idx) {
                status = GrabStatus::kSendPacket;
                return dts2number(pkt.dts);
            }
            av_packet_unref(&pkt);
        }
    }
    return total();
}

int VideoReader::seekKeyFrame(double time, bool backward) {
    return seekKeyFrame(int(fps() * time + 0.5), backward);
}

void VideoReader::init() {
    video_stream_idx = -1;
    fmt_ctx = nullptr;
    video_dec_ctx = nullptr;
    video_stream = nullptr;
    memset(&pkt, 0, sizeof(pkt));
    av_init_packet(&pkt);
    status = GrabStatus::kReadFrame;
}

int VideoReader::dts2number(int64_t dts) const {
    double sec = dts2sec(dts);
    return int(fps() * sec + .5);
}

double VideoReader::dts2sec(int64_t dts) const {
    return (dts - video_stream->start_time) * r2d(video_stream->time_base);
}

double VideoReader::r2d(AVRational r) const {
    return r.num == 0 || r.den == 0 ? 0. : double(r.num) / double(r.den);
}

} // namespace xvision
