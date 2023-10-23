# Sample of Zserio Pub/Sub Mosquitto implementation in C++
cmake_minimum_required(VERSION 3.1.0)

project(ZserioPubsubMosquitto)

option(BUILD_SHARED_LIBS "Whether to build shared libraries" ON)
option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Whether to export all symbols on Windows" ON)

find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h HINTS ENV MOSQUITTO_INCLUDE_DIR REQUIRED)
find_library(MOSQUITTO_LIBRARY NAMES mosquitto HINTS ENV MOSQUITTO_LIBRARY_DIR REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(zserio_download)

download_zserio("${CMAKE_CURRENT_BINARY_DIR}/download" ZSERIO_JAR ZSERIO_RUNTIME_DIR)

# this is necessary for Windows to create runtime dll in the same directory as executables in the build tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_library(${PROJECT_NAME}
    ZserioPubsubMosquitto.cpp
    ZserioPubsubMosquitto.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${ZSERIO_RUNTIME_DIR}
        ${MOSQUITTO_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${MOSQUITTO_LIBRARY})

install(TARGETS ${PROJECT_NAME} DESTINATION ".")
