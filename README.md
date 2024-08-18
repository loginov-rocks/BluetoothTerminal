# BluetoothTerminal

This project aims to implement serial-like communication over BLE, which is available in ESP32-S3 SoC, to communicate
with the [Web Bluetooth Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal) app.

Notes:

* Implements singleton under the hood, because I have not found a way to set ArduinoBLE event handlers without using
  static methods. However, the public class interface does not expose this detail and is used as a regular class. This
  should not have any problems if using a single object.
* Currently supports sending messages up to 20 characters long, where the 20th character is a send separator. A limit
  of 20 characters is the characteristic value size configured. Needs a sending buffer implementation to exceed the
  characteristic value size and write to the characteristic in chunks.
* Capable of receiving messages up to 128 bytes, not including the 128th character since it's needed for the null
  terminator. When the incoming message does not fit the receive buffer the earlier data will be discarded.
* Planned enhancements: make device name, service and characteristic UUIDs, characteristic value size, and receive
  buffer size configurable.

## API

Current API (subject to change as work in progress):

| Method                                                      | Description                                                             |
| ----------------------------------------------------------- | ----------------------------------------------------------------------- |
| `void onConnect(std::function<void(BLEDevice)> handler)`    | Registers a callback for when a BLE device connects.                    |
| `void onDisconnect(std::function<void(BLEDevice)> handler)` | Registers a callback for when a BLE device disconnects.                 |
| `void onReceive(std::function<void(const char*)> handler)`  | Registers a callback for when a message is received.                    |
| `void setReceiveSeparator(char separator)`                  | Sets the character used as the message separator for received messages. |
| `void setSendSeparator(char separator)`                     | Sets the character used as the message separator for sent messages.     |
| `void start()`                                              | Initializes and starts the BLE service and characteristic.              |
| `void loop()`                                               | Polls BLE events; should be called in the `loop()` function.            |
| `bool isConnected()`                                        | Checks if a BLE device is currently connected.                          |
| `void send(const char *message)`                            | Sends a message to the connected BLE device.                            |

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
