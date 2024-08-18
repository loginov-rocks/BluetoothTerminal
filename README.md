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

## Reference

* [ArduinoBLE library](https://github.com/arduino-libraries/ArduinoBLE)
* [ESP32 BLE library](https://github.com/nkolban/ESP32_BLE_Arduino)
