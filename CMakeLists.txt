cmake_minimum_required(VERSION 3.0.0)
project(Lab4 VERSION 0.1.0)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Lab4 task3.cpp)

target_link_libraries(Lab4 ${OpenCV_LIBS})


