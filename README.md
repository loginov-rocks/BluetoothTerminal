# BluetoothTerminal

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/loginov-rocks/library/BluetoothTerminal.svg)](https://registry.platformio.org/libraries/loginov-rocks/BluetoothTerminal)
[![Arduino Lint](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/arduino-lint.yml)
[![PlatformIO Build](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml/badge.svg)](https://github.com/loginov-rocks/BluetoothTerminal/actions/workflows/platformio-build.yml)

The **BluetoothTerminal** library is designed for the **Arduino** framework, providing a simple and efficient way to
implement serial-like communication over **Bluetooth Low Energy** (BLE) on microcontrollers.

Arduino Reference: https://www.arduino.cc/reference/en/libraries/bluetoothterminal/

Built on top of the [ArduinoBLE](https://github.com/arduino-libraries/ArduinoBLE) library, it facilitates seamless
bidirectional data exchange between a **BLE**-enabled **Arduino**-compatible microcontroller and a central device, such
as a smartphone or tablet, using configurable BLE service and characteristic to pass and send messages ending by
configurable separator characters (`\n` by default).

* Supports sending messages longer than the configured characteristic value size (`20` bytes by default) by splitting
  them into chunks and sending with a configurable delay.
* Abstracts away reception of messages longer than the characteristic value size by using a buffer of configurable
  length (`128` bytes by default).
* Provides flexibility to configure service UUID, characteristic UUID, BLE module name, and other used parameters,
  making it adaptable to various applications.

Whether you're developing an IoT application, a remote control system, or any other project requiring reliable
serial-like communication over **BLE** on **Arduino**-compatible microcontrollers, this library offers a way to get
started quickly.

Note: the implementation utilizes a singleton pattern internally due to the necessity of static methods for configuring
[ArduinoBLE](https://github.com/arduino-libraries/ArduinoBLE) event handlers. Despite this, the public interface of the
class remains consistent with standard class usage, and the library is designed to function correctly with a single
instance.

## Quick Start

As a proof of concept, you can use the code below with the
[Web Bluetooth Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal) app in your browser to test
serial-like communication over BLE. The sketch below starts sending a `Hello, world!` message every 5 seconds to the
connected central device and outputs messages sent from the device to the terminal.

```cpp
#include <BluetoothTerminal.h>

BluetoothTerminal bluetoothTerminal;
unsigned long lastMillis = 0;

void handleConnect(BLEDevice device)
{
  Serial.println("Device connected");
}

void handleDisconnect(BLEDevice device)
{
  Serial.println("Device disconnected");
}

void handleReceive(const char *message)
{
  Serial.print("Message received: ");
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);
  bluetoothTerminal.onConnect(handleConnect);
  bluetoothTerminal.onDisconnect(handleDisconnect);
  bluetoothTerminal.onReceive(handleReceive);
  bluetoothTerminal.start();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis > lastMillis + 5000)
  {
    lastMillis = currentMillis;

    if (bluetoothTerminal.isConnected())
    {
      bluetoothTerminal.send("Hello, world!");
    }
  }

  bluetoothTerminal.loop();
}
```

## API

<!-- no toc -->
* [BluetoothTerminal](#bluetoothterminal-1)
  * [void onConnect(std::function<void(BLEDevice)> handler)](#void-onconnectstdfunctionvoidbledevice-handler)
  * [void onDisconnect(std::function<void(BLEDevice)> handler)](#void-ondisconnectstdfunctionvoidbledevice-handler)
  * [void onReceive(std::function<void(const char*)> handler)](#void-onreceivestdfunctionvoidconst-char-handler)
  * [bool setServiceUuid(const char *uuid)](#bool-setserviceuuidconst-char-uuid)
  * [bool setCharacteristicUuid(const char *uuid)](#bool-setcharacteristicuuidconst-char-uuid)
  * [void setCharacteristicValueSize(int size)](#void-setcharacteristicvaluesizeint-size)
  * [bool setName(const char *name)](#bool-setnameconst-char-name)
  * [void setReceiveBufferSize(size_t size)](#void-setreceivebuffersizesize_t-size)
  * [void setReceiveSeparator(char separator)](#void-setreceiveseparatorchar-separator)
  * [void setSendSeparator(char separator)](#void-setsendseparatorchar-separator)
  * [void setSendDelay(int delay)](#void-setsenddelayint-delay)
  * [bool start()](#bool-start)
  * [void loop()](#void-loop)
  * [bool isConnected()](#bool-isconnected)
  * [bool send(const char *message)](#bool-sendconst-char-message)

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

#### `bool setServiceUuid(const char *uuid)`

Sets the BLE service UUID. Returns `true` on success and `false` otherwise.

Optional; if not set, the default UUID `ffe0` will be used. It should be called before `bluetoothTerminal.start()`. If
used after, the method will have no effect. The service UUID can only be set once, if the method is called again, it
will log an error, and the existing service UUID will remain unchanged.

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

#### `bool setCharacteristicUuid(const char *uuid)`

Sets the BLE characteristic UUID. Returns `true` on success and `false` otherwise.

Optional; if not set, the default UUID `ffe1` will be used. It should be called before `bluetoothTerminal.start()`. If
used after, the method will have no effect. The characteristic UUID can only be set once, if the method is called
again, it will log an error, and the existing characteristic UUID will remain unchanged.

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

Sets the size of the BLE characteristic value in bytes.

Optional; if not set, `20` bytes will be used by default. It should be called before `bluetoothTerminal.start()`. If
used after, the method will have no effect.

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

#### `bool setName(const char *name)`

Sets the name of the BLE device that is shown when pairing. Returns `true` on success and `false` otherwise.

Optional; if not set, the default name `Arduino` will be visible only after pairing (according to underlying library
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

Sets the size of the buffer used for receiving data. The buffer accumulates received data until the receive separator
(`\n` by default) is detected, indicating the end of a message. The buffer must be large enough to accommodate the
entire message, including the separator, before processing. If a message exceeds the buffer size, the oldest data will
be discarded to make room for new incoming data, preventing buffer overflow. Setting a buffer size that fits the
expected message lengths is essential to avoid unintended data loss.

Optional; if not set, `128` bytes will be used by default. If called in runtime, for example during `loop()` execution,
and there is data in the buffer, it will be discarded.

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

Sets the separator character used to identify the end of a received message.

Optional; if not set, `\n` will be used by default. Can be changed in runtime, for example during `loop()` execution.

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

Sets the separator character appended to each message sent.

Optional; if not set, `\n` will be used by default. Can be changed in runtime, for example during `loop()` execution.

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

Sets a delay in milliseconds to wait between sending chunks of a message when the total message length, including the
send separator, exceeds the characteristic value size. This delay is useful in cases where some chunks are not received
by the central device (the device connecting to the BLE module).

Optional; if not set, no delay will be used by default. Can be changed in runtime, for example during `loop()`
execution.

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

#### `bool start()`

Initializes and starts the BLE service, making the device discoverable and ready to accept connections. Returns `true`
on success and `false` otherwise.

It is recommended to call this method after completing all other configurations, such as setting the service UUID,
characteristic UUID, device name, and buffer sizes. Once start() is called, changes to these settings will not take
effect. Starting the service after configuration ensures that all settings are correctly applied before the BLE service
becomes active.

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

Handles BLE events and should be called in the main `loop()` of your application.

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

#### `bool send(const char *message)`

Sends a message over BLE to the connected device. If the message exceeds the characteristic value size, it is split
into chunks and sent sequentially with delay if configured. Returns `true` on success and `false` otherwise.

Example:

```cpp
void loop()
{
  // ...
  bluetoothTerminal.send("Hello, world!");
  // ...
}
```

## Logging

The library logs every event into `Serial` like this:

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

When a message is received and it is longer than the receive buffer size (including the receive separator) configured,
the oldest data will be discarded with the following log message:

```
[BluetoothTerminal] Device (address "bc:d0:74:45:13:36") has written 20 bytes into the characteristic.
[BluetoothTerminal]   Receive buffer overflow, data discarded: "Hello, world!".
```

## Tested Boards

* ESP32-S3:
  * [WEMOS LOLIN S3 (V1.0.0)](https://www.wemos.cc/en/latest/s3/s3.html)
