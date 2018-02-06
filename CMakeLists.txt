cmake_minimum_required(VERSION 3.9)
project(egltest)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.63 COMPONENTS system thread REQUIRED)
find_package(EGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV 3 REQUIRED)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)
message("OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
message("OpenCV_LIBRARIES=${OpenCV_LIBRARIES}")
add_executable(
        egltest
        main.cpp
        EglWorker.cpp
)
target_include_directories(
        egltest
        PUBLIC
        ${Boost_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(egltest
        ${EGL_LIBRARIES}
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBRARIES}
        )
