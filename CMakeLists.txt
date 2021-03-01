cmake_minimum_required(VERSION 3.0.0)
project(playground VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 17)
include(CTest)
enable_testing()

find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

include_directories(
    "/usr/local/Cellar/nlohmann_json/3.9.1/include/"
)

add_executable(playground main.cpp)
target_link_libraries(playground 
    PRIVATE nlohmann_json::nlohmann_json fmt::fmt)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
