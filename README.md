# BluetoothTerminal

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/loginov-rocks/library/BluetoothTerminal.svg)](https://registry.platformio.org/libraries/loginov-rocks/BluetoothTerminal)
[![Arduino Lint](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml)
[![PlatformIO Build](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml)

This project aims to implement serial-like communication over BLE, which is available in ESP32-S3 SoC, to communicate
with the [Web Bluetooth Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal) app.

Notes:

* Supports sending messages longer than the characteristic value size (currently hardcoded to 20 bytes) by splitting
  the message into chunks and sending them with optional delay.
* Capable of receiving messages up to 128 bytes (configurable), not including the 128th character since it's needed
  for the null terminator. When the incoming message does not fit the receive buffer the earlier data will be
  discarded.
* Implements singleton under the hood, because I have not found a way to set ArduinoBLE event handlers without using
  static methods. However, the public class interface does not expose this detail and is used as a regular class. This
  should not have any problems if using a single object.

## API

<!-- no toc -->
* [BluetoothTerminal](#bluetoothterminal-1)
  * [void onConnect(std::function<void(BLEDevice)> handler)](#void-onconnectstdfunctionvoidbledevice-handler)
  * [void onDisconnect(std::function<void(BLEDevice)> handler)](#void-ondisconnectstdfunctionvoidbledevice-handler)
  * [void onReceive(std::function<void(const char*)> handler)](#void-onreceivestdfunctionvoidconst-char-handler)
  * [void setServiceUuid(const char *uuid)](#void-setserviceuuidconst-char-uuid)
  * [void setCharacteristicUuid(const char *uuid)](#void-setcharacteristicuuidconst-char-uuid)
  * [void setCharacteristicValueSize(int size)](#void-setcharacteristicvaluesizeint-size)
  * [void setName(const char *name)](#void-setnameconst-char-name)
  * [void setReceiveBufferSize(size_t size)](#void-setreceivebuffersizesize_t-size)
  * [void setReceiveSeparator(char separator)](#void-setreceiveseparatorchar-separator)
  * [void setSendSeparator(char separator)](#void-setsendseparatorchar-separator)
  * [void setSendDelay(int delay)](#void-setsenddelayint-delay)
  * [void start()](#void-start)
  * [void loop()](#void-loop)
  * [bool isConnected()](#bool-isconnected)
  * [void send(const char *message)](#void-sendconst-char-message)

---

### `BluetoothTerminal`

The `BluetoothTerminal` class is a core component of the library that provides an interface for BLE-based serial
communication, handles the connection with the central device, and provides methods to configure BLE and send and
receive data.

Example:

```cpp
BluetoothTerminal bluetoothTerminal;

void setup()
{
  // ...
}

void loop()
{
  // ...
}
```

---

#### `void onConnect(std::function<void(BLEDevice)> handler)`

Sets a callback function to be invoked when a device connects to the BLE module.

Example:

```cpp
void handleConnect(BLEDevice device)
{
  Serial.println("Device connected");
}

void setup()
{
  // ...
  bluetoothTerminal.onConnect(handleConnect);
  // ...
}
```

---

#### `void onDisconnect(std::function<void(BLEDevice)> handler)`

Sets a callback function to be invoked when a device disconnects from the BLE module.

Example:

```cpp
void handleDisconnect(BLEDevice device)
{
  Serial.println("Device disconnected");
}

void setup()
{
  // ...
  bluetoothTerminal.onDisconnect(handleDisconnect);
  // ...
}
```

---

#### `void onReceive(std::function<void(const char*)> handler)`

Sets a callback function to be invoked when a message ending with the receive separator (`\n` by default) is received
from a connected device.

Example:

```cpp
void handleReceive(const char *message)
{
  Serial.print("Message received: ");
  Serial.println(message);
}

void setup()
{
  // ...
  bluetoothTerminal.onReceive(handleReceive);
  // ...
}
```

---

#### `void setServiceUuid(const char *uuid)`

Sets the BLE service UUID. Optional; if not set, the default UUID `ffe0` will be used. It should be called before
`bluetoothTerminal.start()`. If used after, the method will have no effect. The service UUID can only be set once, if
the method is called again, it will log an error, and the existing service UUID will remain unchanged.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setServiceUuid("ffe0");
  // ...
}
```

---

#### `void setCharacteristicUuid(const char *uuid)`

Sets the BLE characteristic UUID. Optional; if not set, the default UUID `ffe1` will be used. It should be called
before `bluetoothTerminal.start()`. If used after, the method will have no effect. The characteristic UUID can only be
set once, if the method is called again, it will log an error, and the existing characteristic UUID will remain
unchanged.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setCharacteristicUuid("ffe1");
  // ...
}
```

---

#### `void setCharacteristicValueSize(int size)`

Sets the size of the BLE characteristic value in bytes. Optional; if not set, `20` bytes will be used by default. It
should be called before `bluetoothTerminal.start()`. If used after, the method will have no effect.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setCharacteristicValueSize(20);
  // ...
}
```

---

#### `void setName(const char *name)`

Sets the name of the BLE device that is shown when pairing. Optional; if not set, the default name `Arduino` will be
visible only after pairing (according to underlying library
[documentation](https://www.arduino.cc/reference/en/libraries/arduinoble/ble.setdevicename/)). It should be called
before `bluetoothTerminal.start()`. If used after, the method will have no effect. The name can only be set once, if
the method is called again, it will log an error, and the existing name will remain unchanged.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setName("BluetoothTerminal");
  // ...
}
```

---

#### `void setReceiveBufferSize(size_t size)`

Set receive buffer size in bytes. Use to accommodate longer messages.

Optional, `128` bytes by default. Can be changed in runtime, for example during `loop()` execution. However, when
changed it discards the current receive buffer data.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setReceiveBufferSize(256);
  // ...
}
```

---

#### `void setReceiveSeparator(char separator)`

Set character serving as a separator when receiving messages.

Optional, `\n` by default. Can be changed in runtime, for example during `loop()` execution.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setReceiveSeparator('\n');
  // ...
}
```

---

#### `void setSendSeparator(char separator)`

Set character serving as a separator when sending messages.

Optional, `\n` by default. Can be changed in runtime, for example during `loop()` execution.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setSendSeparator('\n');
  // ...
}
```

---

#### `void setSendDelay(int delay)`

Set delay in milliseconds to wait in between sending message in chunks, when the message length including the send
separator is longer than the characteristic value size. Use when some chunks of the message are not received by the
central device (connecting to the BLE module).

Optional, no delay is set by default. Can be changed in runtime, for example during `loop()` execution.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.setSendDelay(100);
  // ...
}
```

---

#### `void start()`

Initializes and starts the BLE service and characteristic.

Example:

```cpp
void setup()
{
  // ...
  bluetoothTerminal.start();
  // ...
}
```

---

#### `void loop()`

Polls BLE events; should be called in the `loop()` function.

Example:

```cpp
void loop()
{
  // ...
  bluetoothTerminal.loop();
  // ...
}
```

---

#### `bool isConnected()`

Checks if a BLE device is currently connected.

Example:

```cpp
void loop()
{
  // ...
  if (bluetoothTerminal.isConnected())
  {
    Serial.println("Device is connected!");
  }
  else
  {
    Serial.println("Device is not connected!");
  }
  // ...
}
```

---

#### `void send(const char *message)`

Sends a message to the connected BLE device.

Example:

```cpp
void loop()
{
  // ...
  bluetoothTerminal.send("Hello, world!");
  // ...
}
```

---

## Logging

The library logs events into `Serial`, for example:

```
[BluetoothTerminal] The connect handler is set.
[BluetoothTerminal] The disconnect handler is set.
[BluetoothTerminal] The receive handler is set.
[BluetoothTerminal] The service UUID is set to "ffe0".
[BluetoothTerminal] The characteristic UUID is set to "ffe1".
[BluetoothTerminal] The characteristic value size is set to 20 bytes.
[BluetoothTerminal] The name is set to "BluetoothTerminal".
[BluetoothTerminal] The receive buffer size is set to 256 bytes.
[BluetoothTerminal] The receive separator is set to "
".
[BluetoothTerminal] The send separator is set to "
".
[BluetoothTerminal] The send delay is set to 100 milliseconds.
[BluetoothTerminal] Starting BLE service... successful.
[BluetoothTerminal] Setting up BLE service and characteristic...
[BluetoothTerminal] BLE service and characteristic were set up.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") was connected.
[BluetoothTerminal] Sending message: "Hello, world!"... sent.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 14 bytes into the characteristic.
[BluetoothTerminal]   Message received: "Hello, world!".
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") was disconnected.
```

## Testing

With message longer than the characteristic value size (20 bytes by default), but less than the receive buffer size
(128 bytes by default):

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
[BluetoothTerminal]   Message received: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.".
```

With message longer than the receive buffer size (128 bytes by default):

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
[BluetoothTerminal]   Receive buffer overflow, data discarded: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut ".
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 12 bytes into the characteristic.
[BluetoothTerminal]   Message received: "enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.".
```

## Reference

* Underlying library: [ArduinoBLE library](https://github.com/arduino-libraries/ArduinoBLE)
* Initially library used: [ESP32 BLE library](https://github.com/nkolban/ESP32_BLE_Arduino)
