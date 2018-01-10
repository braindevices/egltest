cmake_minimum_required(VERSION 3.9)
project(egltest)

set(CMAKE_CXX_STANDARD 14)
include(FindEGL.cmake)
add_executable(egltest main.cpp)
target_link_libraries(egltest
        ${EGL_LIBRARIES}
        )
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${EGL_INCLUDE_DIRS}
)
