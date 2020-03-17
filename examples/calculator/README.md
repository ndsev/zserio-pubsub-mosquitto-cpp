# Calculator Example

This example demonstrates how to use **Zserio Pub/Sub Mosquitto backend** to use Pub/Sub communication
with [mosquitto](https://mosquitto.org/) broker.

## Prerequisites

1. [Mosquitto](https://mosquitto.org) message broker running.
   > On Ubuntu check `systemctl status mosquitto`.
2. Zserio C++ runtime library.
3. Zserio compiler (`zserio.jar`)

> Zserio prerequisites are included in this repo in 3rdparty folder.

## Building

```bash
mkdir build
pushd build
cmake ..
popd
```

## Testing

```bash
cd build
./PowerOfTwoProvider &
./SquareRootOfProvider &
./CalculatorClient
# follow client's instructions
# ...
# pres q + ENTER to quit the client
fg # and press Ctrl+C to quit SquareRootOfProvider
fg # and press Ctrl+C to quit PowerOfTwoProvider
```

> For more understandable output run both providers and client in a separate terminal.
