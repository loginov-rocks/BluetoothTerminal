#ifndef BLUETOOTHTERMINAL_H
#define BLUETOOTHTERMINAL_H

#include <ArduinoBLE.h>

class BluetoothTerminal
{
public:
  static BluetoothTerminal &getInstance();

  using ConnectHandler = std::function<void(BLEDevice)>;
  void onConnect(ConnectHandler);
  using DisconnectHandler = std::function<void(BLEDevice)>;
  void onDisconnect(DisconnectHandler);
  using ReceiveHandler = std::function<void(const char *)>;
  void onReceive(ReceiveHandler);

  void setReceiveSeparator(char);
  void setSendSeparator(char);

  void start();
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

  ConnectHandler connectHandler;
  DisconnectHandler disconnectHandler;
  ReceiveHandler receiveHandler;

  char receiveSeparator = '\n';
  char sendSeparator = '\n';

  static void handleConnectedStatic(BLEDevice);
  static void handleDisconnectedStatic(BLEDevice);
  static void handleWrittenStatic(BLEDevice, BLECharacteristic);

  void handleConnected(BLEDevice);
  void handleDisconnected(BLEDevice);
  void handleWritten(BLEDevice, BLECharacteristic);
};

#endif
