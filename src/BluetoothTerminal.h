#ifndef BLUETOOTHTERMINAL_H
#define BLUETOOTHTERMINAL_H

#include <ArduinoBLE.h>

class BluetoothTerminal
{
public:
  static BluetoothTerminal &getInstance();

  void setReceiveSeparator(char);
  void setSendSeparator(char);

  void setup();
  void loop();

  bool isConnected();
  void send(const char *);

private:
  BLEService bleService = BLEService("ffe0");
  BLECharacteristic bleCharacteristic = BLECharacteristic("ffe1", BLERead | BLEWriteWithoutResponse | BLENotify, 20);
  int characteristicValueSize = 20;

  bool deviceConnected = false;

  char receiveBuffer[128];
  int receiveBufferIndex = 0;
  int receiveBufferSize = 128;

  char receiveSeparator = '\n';
  char sendSeparator = '\n';

  static void onConnectedStatic(BLEDevice);
  static void onDisconnectedStatic(BLEDevice);
  static void onWrittenStatic(BLEDevice, BLECharacteristic);

  void onConnected(BLEDevice);
  void onDisconnected(BLEDevice);
  void onWritten(BLEDevice, BLECharacteristic);
};

#endif
