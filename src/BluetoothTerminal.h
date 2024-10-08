#ifndef BLUETOOTHTERMINAL_H
#define BLUETOOTHTERMINAL_H

#include <ArduinoBLE.h>

class BluetoothTerminal
{
public:
  ~BluetoothTerminal();

  void enableDebug();
  void disableDebug();

  using ConnectHandler = std::function<void(BLEDevice)>;
  void onConnect(ConnectHandler);
  using DisconnectHandler = std::function<void(BLEDevice)>;
  void onDisconnect(DisconnectHandler);
  using ReceiveHandler = std::function<void(const char *)>;
  void onReceive(ReceiveHandler);

  bool setServiceUuid(const char *);
  bool setCharacteristicUuid(const char *);
  void setCharacteristicValueSize(int);

  bool setName(const char *);
  void setReceiveBufferSize(size_t size);
  void setReceiveSeparator(char);
  void setSendSeparator(char);
  void setSendDelay(int);

  bool start();
  void loop();

  bool isConnected();
  bool send(const char *);

private:
  bool isDebugEnabled = false;

  BLEService *bleService = nullptr;
  BLECharacteristic *bleCharacteristic = nullptr;

  bool deviceConnected = false;

  char *receiveBuffer = nullptr;
  int receiveBufferIndex = 0;
  bool receiveBufferInitialized = false;

  ConnectHandler connectHandler;
  DisconnectHandler disconnectHandler;
  ReceiveHandler receiveHandler;

  char *serviceUuid = nullptr;        // `ffe0` used by default.
  char *characteristicUuid = nullptr; // `ffe1` used by default.
  int characteristicValueSize = 20;

  char *name = nullptr;
  size_t receiveBufferSize = 128;
  char receiveSeparator = '\n';
  char sendSeparator = '\n';
  int sendDelay = 0;

  static BluetoothTerminal &getInstance();

  static void handleConnectedStatic(BLEDevice);
  static void handleDisconnectedStatic(BLEDevice);
  static void handleWrittenStatic(BLEDevice, BLECharacteristic);

  void handleConnected(BLEDevice);
  void handleDisconnected(BLEDevice);
  void handleWritten(BLEDevice, BLECharacteristic);
};

#endif
