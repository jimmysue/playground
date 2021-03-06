cmake_minimum_required(VERSION 3.0.0)
project(playground VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (CMAKE_CXX_STANDARD 17)
include(CTest)
enable_testing()


include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(xvision)