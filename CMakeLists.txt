cmake_minimum_required(VERSION 3.19)
project(AclerisApp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_executable(AclerisApp main.cpp)

add_subdirectory(acleris)

target_link_libraries(AclerisApp Acleris)