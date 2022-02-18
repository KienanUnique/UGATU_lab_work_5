cmake_minimum_required(VERSION 3.21)
project(UGATU_lab_work_5)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(UGATU_lab_work_5 main.cpp)

target_link_libraries(UGATU_lab_work_5 sfml-graphics)