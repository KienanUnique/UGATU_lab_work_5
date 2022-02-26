cmake_minimum_required(VERSION 3.21)
set(CMAKE_PREFIX_PATH "C:/Users/wi/Documents/CppLibraries")

project(UGATU_lab_work_5)

set(CMAKE_CXX_STANDARD 14)
set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
add_executable(UGATU_lab_work_5 main.cpp)

target_link_libraries(UGATU_lab_work_5 sfml-graphics sfml-window sfml-system)