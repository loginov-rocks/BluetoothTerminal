#include <BluetoothTerminal.h>

BluetoothTerminal &BluetoothTerminal::getInstance()
{
  // This instance will be created once and reused.
  static BluetoothTerminal instance;

  return instance;
}

void BluetoothTerminal::begin()
{
  Serial.print("[BluetoothTerminal] Starting BLE service...");
  if (!BLE.begin())
  {
    Serial.println(" failed!");
    while (true)
      ;
  }
  Serial.println(" successful!");
  Serial.println("[BluetoothTerminal] Setting up BLE service and characteristic...");

  BLE.setLocalName("ESP32");
  BLE.setDeviceName("ESP32");

  BLE.setEventHandler(BLEConnected, BluetoothTerminal::onConnectStatic);
  BLE.setEventHandler(BLEDisconnected, BluetoothTerminal::onDisconnectStatic);

  BluetoothTerminal instance = BluetoothTerminal::getInstance();

  instance.bleService.addCharacteristic(instance.bleCharacteristic);
  instance.bleCharacteristic.setEventHandler(BLEWritten, BluetoothTerminal::onWriteStatic);
  BLE.addService(instance.bleService);

  BLE.setAdvertisedService(instance.bleService);
  BLE.advertise();

  Serial.println("[BluetoothTerminal] BLE service and characteristic are set up!");
}

void BluetoothTerminal::loop()
{
  BLE.poll();
}

void BluetoothTerminal::send(const char *message)
{
  Serial.print("[BluetoothTerminal] Sending message: \"");
  Serial.print(message);
  Serial.print("\"...");

  if (!this->isDeviceConnected())
  {
    Serial.println(" failed, no device connected!");
  }

  BluetoothTerminal::getInstance().bleCharacteristic.setValue(message);

  Serial.println(" sent!");
}

bool BluetoothTerminal::isDeviceConnected()
{
  return BluetoothTerminal::getInstance().deviceConnected;
}

void BluetoothTerminal::onConnectStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().onConnect(device);
}

void BluetoothTerminal::onDisconnectStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().onDisconnect(device);
}

void BluetoothTerminal::onWriteStatic(BLEDevice device, BLECharacteristic characteristic)
{
  BluetoothTerminal::getInstance().onWrite(device, characteristic);
}

/**
 * This method is called from the static method, so `this` references the singleton instance.
 */
void BluetoothTerminal::onConnect(BLEDevice device)
{
  this->deviceConnected = true;

  Serial.print("[BluetoothTerminal] Device with address \"");
  Serial.print(device.address());
  Serial.println("\" connected!");
}

/**
 * This method is called from the static method, so `this` references the singleton instance.
 */
void BluetoothTerminal::onDisconnect(BLEDevice device)
{
  this->deviceConnected = false;

  Serial.print("[BluetoothTerminal] Device with address \"");
  Serial.print(device.address());
  Serial.println("\" disconnected!");
}

/**
 * This method is called from the static method, so `this` references the singleton instance.
 */
void BluetoothTerminal::onWrite(BLEDevice device, BLECharacteristic characteristic)
{
  Serial.print("[BluetoothTerminal] The connected device with address \"");
  Serial.print(device.address());
  Serial.println("\" has written to the characteristic!");

  const uint8_t *value = characteristic.value();
  int length = characteristic.valueLength();

  for (int i = 0; i < length; i++)
  {
    const char _char = (char)value[i];
    this->buffer[this->bufferIndex++] = _char;

    if (_char == '\n' || bufferIndex == this->bufferSize - 1)
    {
      this->buffer[this->bufferIndex] = '\0';

      Serial.print("[BluetoothTerminal] Message received: \"");
      Serial.print(this->buffer);
      Serial.println("\"");

      this->bufferIndex = 0;
    }
  }
}
