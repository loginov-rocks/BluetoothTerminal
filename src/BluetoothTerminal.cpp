#include <BluetoothTerminal.h>

BluetoothTerminal &BluetoothTerminal::getInstance()
{
  // This instance will be created once and reused.
  static BluetoothTerminal instance;

  return instance;
}

void BluetoothTerminal::setup()
{
  Serial.print("[BluetoothTerminal] Starting BLE service...");
  if (!BLE.begin())
  {
    Serial.println(" failed!");
    return;
  }
  Serial.println(" successful.");
  Serial.println("[BluetoothTerminal] Setting up BLE service and characteristic...");

  // TODO: Make configurable.
  BLE.setLocalName("ESP32");
  BLE.setDeviceName("ESP32");

  BLE.setEventHandler(BLEConnected, BluetoothTerminal::onConnectedStatic);
  BLE.setEventHandler(BLEDisconnected, BluetoothTerminal::onDisconnectedStatic);

  BluetoothTerminal instance = BluetoothTerminal::getInstance();

  instance.bleService.addCharacteristic(instance.bleCharacteristic);
  instance.bleCharacteristic.setEventHandler(BLEWritten, BluetoothTerminal::onWrittenStatic);
  BLE.addService(instance.bleService);

  BLE.setAdvertisedService(instance.bleService);
  BLE.advertise();

  Serial.println("[BluetoothTerminal] BLE service and characteristic were set up.");
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

  BluetoothTerminal instance = BluetoothTerminal::getInstance();

  if (!instance.isConnected())
  {
    Serial.println(" failed, no device connected!");
    return;
  }

  size_t length = strlen(message);

  // TODO: Implement a send buffer to accommodate messages larger than the
  // characteristic value size.
  if (length > instance.characteristicValueSize - 1)
  {
    Serial.print(" failed, the message length should be less than ");
    Serial.print(instance.characteristicValueSize);
    Serial.println(" characters!");
    return;
  }

  // One byte for the send separator, another for the null terminator. Can
  // create string longer than the characteristic value size by one byte
  // accomodating the null terminator, however during testing did not find any
  // problems.
  char messageWithSeparator[length + 2];
  strcpy(messageWithSeparator, message);
  messageWithSeparator[length] = instance.sendSeparator;
  messageWithSeparator[length + 1] = '\0';

  instance.bleCharacteristic.setValue(messageWithSeparator);

  Serial.println(" sent.");
}

bool BluetoothTerminal::isConnected()
{
  return BluetoothTerminal::getInstance().deviceConnected;
}

void BluetoothTerminal::setReceiveSeparator(char separator)
{
  BluetoothTerminal::getInstance().receiveSeparator = separator;
}

void BluetoothTerminal::setSendSeparator(char separator)
{
  BluetoothTerminal::getInstance().sendSeparator = separator;
}

void BluetoothTerminal::onConnectedStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().onConnected(device);
}

void BluetoothTerminal::onDisconnectedStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().onDisconnected(device);
}

void BluetoothTerminal::onWrittenStatic(BLEDevice device, BLECharacteristic characteristic)
{
  BluetoothTerminal::getInstance().onWritten(device, characteristic);
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::onConnected(BLEDevice device)
{
  this->deviceConnected = true;

  Serial.print("[BluetoothTerminal] Device (address \"");
  Serial.print(device.address());
  Serial.println("\") was connected.");
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::onDisconnected(BLEDevice device)
{
  this->deviceConnected = false;

  Serial.print("[BluetoothTerminal] Device (address \"");
  Serial.print(device.address());
  Serial.println("\") was disconnected.");
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::onWritten(BLEDevice device, BLECharacteristic characteristic)
{
  const uint8_t *value = characteristic.value();
  int length = characteristic.valueLength();

  Serial.print("[BluetoothTerminal] Device (address \"");
  Serial.print(device.address());
  Serial.print("\") has written ");
  Serial.print(length);
  Serial.println(" bytes into the characteristic.");

  for (int i = 0; i < length; i++)
  {
    const char _char = (char)value[i];

    // The separator was received, output the buffer as a message, reset the
    // buffer, and continue the for loop.
    if (_char == this->receiveSeparator)
    {
      this->receiveBuffer[this->receiveBufferIndex] = '\0';

      Serial.print("[BluetoothTerminal] Message received: \"");
      Serial.print(this->receiveBuffer);
      Serial.println("\".");

      this->receiveBufferIndex = 0;

      continue;
    }

    // If the separator was not received and the receive buffer cannot
    // accommodate additional characters, discard data in the buffer and reset.
    if (this->receiveBufferIndex > this->receiveBufferSize - 2)
    {
      this->receiveBuffer[this->receiveBufferIndex] = '\0';

      Serial.print("[BluetoothTerminal] Receive buffer overflow, data discarded: \"");
      Serial.print(this->receiveBuffer);
      Serial.println("\".");

      this->receiveBufferIndex = 0;
    }

    this->receiveBuffer[this->receiveBufferIndex++] = _char;
  }
}
