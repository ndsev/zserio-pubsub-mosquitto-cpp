# Zserio Pub/Sub Mosquitto Backend

[![](https://github.com/ndsev/zserio-pubsub-mosquitto-cpp/actions/workflows/build_linux.yml/badge.svg)](https://github.com/ndsev/zserio-pubsub-mosquitto-cpp/actions/workflows/build_linux.yml)
[![](https://github.com/ndsev/zserio-pubsub-mosquitto-cpp/actions/workflows/build_windows.yml/badge.svg)](https://github.com/ndsev/zserio-pubsub-mosquitto-cpp/actions/workflows/build_windows.yml)
[![](https://img.shields.io/github/watchers/ndsev/zserio-pubsub-mosquitto-cpp.svg)](https://GitHub.com/ndsev/zserio-pubsub-mosquitto-cpp/watchers)
[![](https://img.shields.io/github/forks/ndsev/zserio-pubsub-mosquitto-cpp.svg)](https://GitHub.com/ndsev/zserio-pubsub-mosquitto-cpp/network/members)
[![](https://img.shields.io/github/stars/ndsev/zserio-pubsub-mosquitto-cpp.svg?color=yellow)](https://GitHub.com/ndsev/zserio-pubsub-mosquitto-cpp/stargazers)

--------

Sample implementation of Zserio Pub/Sub mosquitto backend in **C++**.

## Prerequisites

1. Install [Mosquitto](https://mosquitto.org) according to
[official instructions](https://mosquitto.org/download/). Mosquitto is the message broker which implements
MQTT protocol.

2. Install C library libmosquitto for development. E.g. on Ubuntu install `libmosquitto-dev`.

> Note that Zserio prerequisites are automatically downloaded by CMake.

## Building

Go to the zserio-pubsub-mosquitto-cpp root directory.

Build ZserioPubsubMosquitto library:
```bash
mkdir build
pushd build
cmake ../src
cmake --build .
popd
```

## Usage

See [Calculator Example](./examples/calculator/README.md).
