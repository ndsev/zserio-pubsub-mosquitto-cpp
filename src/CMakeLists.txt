# Sample of Zserio Pub/Sub Mosquitto implementation in C++
cmake_minimum_required(VERSION 2.8.12.2)

project(ZserioPubsubMosquitto)

option(BUILD_SHARED_LIBS "Whether to build shared libraries" ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(zserio_download)

download_zserio("${CMAKE_CURRENT_BINARY_DIR}/download" ZSERIO_JAR ZSERIO_RUNTIME_DIR)

add_library(${PROJECT_NAME}
    ZserioPubsubMosquitto.cpp
    ZserioPubsubMosquitto.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${ZSERIO_RUNTIME_DIR})
target_link_libraries(${PROJECT_NAME} -lmosquitto)
