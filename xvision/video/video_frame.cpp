#include "video_frame.h"

#include <assert.h>

#include <utility>

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

VideoFrame::VideoFrame(VideoFrame &&v) : VideoFrame() {
    std::swap(_ptr, v._ptr); // make VideoFrame empty after moved
}

VideoFrame &VideoFrame::operator=(const VideoFrame &v) {
    if (this != &v) {
        release();
        av_frame_ref(_ptr, v._ptr);
    }
    return *this;
}

VideoFrame &VideoFrame::operator=(VideoFrame &&v) {
    if (this != &v) { // prevent self move
        release();
        std::swap(_ptr, v._ptr);
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

} // namespace xvision
