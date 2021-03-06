
find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
add_executable(test_video_frame test_video_frame.cpp)
target_link_libraries(test_video_frame PRIVATE video ${OpenCV_LIBS})
