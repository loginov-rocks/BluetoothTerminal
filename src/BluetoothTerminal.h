#ifndef BLUETOOTHTERMINAL_H
#define BLUETOOTHTERMINAL_H

#include <ArduinoBLE.h>

class BluetoothTerminal
{
public:
  static BluetoothTerminal &getInstance();

  void begin();
  void loop();
  void send(const char *);
  bool isDeviceConnected();

private:
  BLEService bleService = BLEService("ffe0");
  BLECharacteristic bleCharacteristic = BLECharacteristic("ffe1", BLERead | BLEWriteWithoutResponse | BLENotify, 20);
  char buffer[128];
  int bufferIndex = 0;
  int bufferSize = 128;
  bool deviceConnected = false;

  static void onConnectStatic(BLEDevice);
  static void onDisconnectStatic(BLEDevice);
  static void onWriteStatic(BLEDevice, BLECharacteristic);

  void onConnect(BLEDevice);
  void onDisconnect(BLEDevice);
  void onWrite(BLEDevice, BLECharacteristic);
};

#endif
