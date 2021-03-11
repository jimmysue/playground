
find_package(FFmpeg REQUIRED)
find_package(libyuv REQUIRED)
find_package(OpenCV REQUIRED)

set(SOURCES 
    "video_frame.cpp"
    "video_reader.cpp"
)

include_directories(
    ${FFMPEG_INCLUDE_DIRS}
    ${LibYUV_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

message(STATUS "ffmpeg includes: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "==> ${LibYUV_LIBRARIES}")
message(STATUS "==> ${FFMPEG_LIBRARIES}")

add_library(video ${SOURCES})
target_link_libraries(video ${FFMPEG_LIBRARIES} ${LibYUV_LIBRARIES} ${OpenCV_LIBS})

if(BUILD_TESTING) 
add_subdirectory(test)
endif()