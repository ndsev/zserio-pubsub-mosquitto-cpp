# Zserio Pub/Sub Mosquitto Backend

Sample implementation of Zserio Pub/Sub mosquitto backend in **C++**.

## Prerequisites

Install [Mosquitto](https://mosquitto.org) according to
[official instructions](https://mosquitto.org/download/). Mosquitto is the message broker which implements
MQTT protocol.

> This sample backend implementation is based on C library libmosquitto.
E.g. on Ubuntu install libmosquitto-dev.

## Building

Go to the zserio-pubsub-mosquitto-cpp root directory.

Build ZserioPubsubMosquitto library:
```bash
mkdir build
pushd build
cmake ../src
make
popd
```

## Usage
