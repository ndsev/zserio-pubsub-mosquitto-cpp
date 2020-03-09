# Sample of Zserio Pub/Sub Mosquitto implementation in C++
cmake_minimum_required(VERSION 2.8.12.2)

project(ZserioPubsubMosquitto)

set(REPOSITORY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH "")
set(ZSERIO_INCLUDE_DIR "${REPOSITORY_ROOT}/3rdparty/runtime" CACHE PATH "")
option(BUILD_SHARED_LIBS "Whether to build shared libraries" ON)

add_library(${PROJECT_NAME}
    ZserioPubsubMosquitto.cpp
    ZserioPubsubMosquitto.h
)
target_link_libraries(ZserioPubsubMosquitto -lmosquitto)
target_include_directories(ZserioPubsubMosquitto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${ZSERIO_INCLUDE_DIR})
