
find_package(FFmpeg REQUIRED)

set(SOURCES 
    "video_frame.cpp"
    "video_reader.cpp"
)

include_directories(
    ${FFMPEG_INCLUDE_DIRS}
)

message(STATUS "ffmpeg includes: ${FFMPEG_INCLUDE_DIRS}")

add_library(video ${SOURCES})
target_link_libraries(video ${FFMPEG_LIBRARIES})

if(BUILD_TESTING) 
add_subdirectory(test)
endif()