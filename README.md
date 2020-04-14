# Zserio Pub/Sub Mosquitto Backend

Sample implementation of Zserio Pub/Sub mosquitto backend in **C++**.

## Prerequisites

1. Install [Mosquitto](https://mosquitto.org) according to
[official instructions](https://mosquitto.org/download/). Mosquitto is the message broker which implements
MQTT protocol.

2. Install C library libmosquitto for development. E.g. on Ubuntu install `libmosquitto-dev`.

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
