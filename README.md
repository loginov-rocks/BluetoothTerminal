# BluetoothTerminal

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/loginov-rocks/library/BluetoothTerminal.svg)](https://registry.platformio.org/libraries/loginov-rocks/BluetoothTerminal)
[![Arduino Lint](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml)
[![PlatformIO Build](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml)

This project aims to implement serial-like communication over BLE, which is available in ESP32-S3 SoC, to communicate
with the [Web Bluetooth Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal) app.

Notes:

* Supports sending messages longer than the characteristic value size (currently hardcoded to 20 bytes) by splitting
  the message into chunks and sending them with optional delay.
* Capable of receiving messages up to 128 bytes, not including the 128th character since it's needed for the null
  terminator. When the incoming message does not fit the receive buffer the earlier data will be discarded.
* Implements singleton under the hood, because I have not found a way to set ArduinoBLE event handlers without using
  static methods. However, the public class interface does not expose this detail and is used as a regular class. This
  should not have any problems if using a single object.
* Planned enhancements: make device name, service and characteristic UUIDs, characteristic value size, and receive
  buffer size configurable.

## API

Current API (subject to change as work in progress):

| Method                                                      | Description                                                               |
| ----------------------------------------------------------- | ------------------------------------------------------------------------- |
| `void onConnect(std::function<void(BLEDevice)> handler)`    | Registers a callback for when a BLE device connects.                      |
| `void onDisconnect(std::function<void(BLEDevice)> handler)` | Registers a callback for when a BLE device disconnects.                   |
| `void onReceive(std::function<void(const char*)> handler)`  | Registers a callback for when a message is received.                      |
| `void setReceiveSeparator(char separator)`                  | Sets the character used as the message separator for received messages.   |
| `void setSendSeparator(char separator)`                     | Sets the character used as the message separator for sent messages.       |
| `void setSendDelay(int delay)`                              | Sets the delay in milliseconds used in between sending message in chunks. |
| `void start()`                                              | Initializes and starts the BLE service and characteristic.                |
| `void loop()`                                               | Polls BLE events; should be called in the `loop()` function.              |
| `bool isConnected()`                                        | Checks if a BLE device is currently connected.                            |
| `void send(const char *message)`                            | Sends a message to the connected BLE device.                              |

## Testing

Sample serial messaging:

```
[BluetoothTerminal] Starting BLE service... successful.
[BluetoothTerminal] Setting up BLE service and characteristic...
[BluetoothTerminal] BLE service and characteristic were set up.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") was connected.
[BluetoothTerminal] Sending message: "Hello, world!"... sent.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 14 bytes into the characteristic.
[BluetoothTerminal] Message received: "Hello, world!".
```

Message longer than the characteristic value size, but less than the receive buffer size (128 bytes):

```
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
```

```
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 4 bytes into the characteristic.
[BluetoothTerminal] Message received: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.".
```

Message longer than the receive buffer size (128 bytes):

```
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
```

```
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Receive buffer overflow, data discarded: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut ".
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 12 bytes into the characteristic.
[BluetoothTerminal] Message received: "enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.".
```

## Reference

* [ArduinoBLE library](https://github.com/arduino-libraries/ArduinoBLE)
* [ESP32 BLE library](https://github.com/nkolban/ESP32_BLE_Arduino)
