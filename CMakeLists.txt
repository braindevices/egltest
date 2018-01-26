cmake_minimum_required(VERSION 3.9)
project(egltest)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.63 COMPONENTS thread REQUIRED)
find_package(EGL REQUIRED)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)
add_executable(
        egltest
        main.cpp
        multithreadegl.cpp
)
target_include_directories(
        egltest
        PUBLIC
        ${Boost_INCLUDE_DIR}
)
target_link_libraries(egltest
        ${EGL_LIBRARIES}
        ${Boost_THREAD_LIBRARY}
        )
