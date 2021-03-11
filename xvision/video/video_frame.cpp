#include "video_frame.h"

#include <assert.h>

#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace xvision {

VideoFrame::VideoFrame() {
    _ptr = av_frame_alloc();
    assert(_ptr);
}

VideoFrame::VideoFrame(int width, int height, PixelFormat fmt) : VideoFrame() {
    create(width, height, fmt);
}

VideoFrame::~VideoFrame() { av_frame_free(&_ptr); }

VideoFrame::VideoFrame(const VideoFrame &v) : VideoFrame() {
    av_frame_ref(_ptr, v._ptr);
}

VideoFrame::VideoFrame(VideoFrame &&v) : VideoFrame() { swap(v); }

VideoFrame &VideoFrame::operator=(const VideoFrame &v) {
    if (this != &v) {
        this->release();
        av_frame_ref(_ptr, v._ptr);
    }
    return *this;
}

VideoFrame &VideoFrame::operator=(VideoFrame &&v) {
    if (this != &v) {
        this->release();
        swap(v);
    }
    return *this;
}

void VideoFrame::swap(VideoFrame &v) noexcept { std::swap(_ptr, v._ptr); }

void VideoFrame::create(int width, int height, PixelFormat fmt) {
    release();
    _ptr->width = width;
    _ptr->height = height;
    _ptr->format = fmt;
    av_frame_get_buffer(_ptr, 32);
}

void VideoFrame::release() { av_frame_unref(_ptr); }

VideoFrame VideoFrame::clone() const {
    VideoFrame res(_ptr->width, _ptr->height, PixelFormat(_ptr->format));
    av_frame_copy_props(res._ptr, _ptr);
    av_frame_copy(res._ptr, _ptr);
}

bool VideoFrame::empty() const { return _ptr->width <= 0 || _ptr->height <= 0; }

////////////////////////////////////////////////////////////////////
VideoFrame VideoFrame::convert(PixelFormat fmt) const {}

VideoFrame VideoFrame::scale(int width, int height) const {
    VideoFrame ret;
    switch (_ptr->format) {
    case PixelFormat::AV_PIX_FMT_YUV420P:
    case PixelFormat::AV_PIX_FMT_YUVJ420P: {
        ret.create(width, height, PixelFormat(_ptr->format));
        libyuv::I420Scale(_ptr->data[0], _ptr->linesize[0], _ptr->data[1],
                          _ptr->linesize[1], _ptr->data[2], _ptr->linesize[2],
                          _ptr->width, _ptr->height, ret->data[0],
                          ret->linesize[0], ret->data[1], ret->linesize[1],
                          ret->data[2], ret->linesize[2], width, height,
                          libyuv::FilterMode::kFilterBilinear);
        break;
    }
    default:
        throw std::runtime_error("not supported");
    }
    return ret;
}

VideoFrame VideoFrame::rotate(RotationMode mode) const {
    if (mode == RotationMode::kRotate0) {
        return *this;
    }
    VideoFrame dst;
    if (mode == RotationMode::kRotate90 || mode == RotationMode::kRotate270) {
        dst.create(_ptr->height, _ptr->width, PixelFormat(_ptr->format));
    } else {
        dst.create(_ptr->width, _ptr->height, PixelFormat(_ptr->format));
    }
    switch (_ptr->format) {
    case PixelFormat::AV_PIX_FMT_YUV420P:
    case PixelFormat::AV_PIX_FMT_YUVJ420P:
        libyuv::I420Rotate(_ptr->data[0], _ptr->linesize[0], _ptr->data[1],
                           _ptr->linesize[1], _ptr->data[2], _ptr->linesize[2],
                           dst->data[0], dst->linesize[0], dst->data[1],
                           dst->linesize[1], dst->data[2], dst->linesize[2],
                           _ptr->width, _ptr->height, mode);
        break;
    default:
        throw std::runtime_error("not supported");
    }
    return dst;
}

cv::Mat VideoFrame::mat(int flag) const {
    cv::Mat dst;
    switch (_ptr->format) {
    case PixelFormat::AV_PIX_FMT_YUV420P:
    case PixelFormat::AV_PIX_FMT_YUVJ420P:
        if (flag == cv::IMREAD_COLOR) {
            dst.create(_ptr->height, _ptr->width, CV_8UC3);
            if (_ptr->format == PixelFormat::AV_PIX_FMT_YUV420P) {
                libyuv::I420ToRGB24(_ptr->data[0], _ptr->linesize[0],
                                    _ptr->data[1], _ptr->linesize[1],
                                    _ptr->data[2], _ptr->linesize[2], dst.data,
                                    dst.step, _ptr->width, _ptr->height);
            } else {
                libyuv::J420ToRGB24(_ptr->data[0], _ptr->linesize[0],
                                    _ptr->data[1], _ptr->linesize[1],
                                    _ptr->data[2], _ptr->linesize[2], dst.data,
                                    dst.step, _ptr->width, _ptr->height);
            }
            return dst;
        } else if (flag == cv::IMREAD_GRAYSCALE) {
            return cv::Mat(_ptr->height, _ptr->width, CV_8UC1, _ptr->data[0],
                           _ptr->linesize[0])
                .clone();
        }
        break;
    default:
        throw std::runtime_error("not supported");
    }
    return dst;
}

} // namespace xvision
