cmake_minimum_required(VERSION 3.5)

project(opencv_train LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(photo)
add_subdirectory(train)
add_subdirectory(recognition)

add_subdirectory(face_detection_yunet)
