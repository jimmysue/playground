find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
add_executable(gauss_newton gauss_newton.cpp)
target_link_libraries(gauss_newton Eigen3::Eigen ${OpenCV_LIBS})