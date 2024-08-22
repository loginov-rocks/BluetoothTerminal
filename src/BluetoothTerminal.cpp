#include <BluetoothTerminal.h>

BluetoothTerminal::~BluetoothTerminal()
{
  delete[] name;
}

BluetoothTerminal &BluetoothTerminal::getInstance()
{
  // This instance will be created once and reused.
  static BluetoothTerminal instance;

  return instance;
}

void BluetoothTerminal::onConnect(ConnectHandler handler)
{
  BluetoothTerminal::getInstance().connectHandler = handler;
}

void BluetoothTerminal::onDisconnect(DisconnectHandler handler)
{
  BluetoothTerminal::getInstance().disconnectHandler = handler;
}

void BluetoothTerminal::onReceive(ReceiveHandler handler)
{
  BluetoothTerminal::getInstance().receiveHandler = handler;
}

void BluetoothTerminal::setName(const char *name)
{
  if (name != nullptr)
  {
    BluetoothTerminal instance = BluetoothTerminal::getInstance();
    if (instance.name != nullptr)
    {
      delete[] instance.name;
    }

    size_t length = strlen(name) + 1;
    instance.name = new char[length];
    strncpy(instance.name, name, length);
  }
}

void BluetoothTerminal::setReceiveSeparator(char separator)
{
  BluetoothTerminal::getInstance().receiveSeparator = separator;
}

void BluetoothTerminal::setSendSeparator(char separator)
{
  BluetoothTerminal::getInstance().sendSeparator = separator;
}

void BluetoothTerminal::setSendDelay(int delay)
{
  BluetoothTerminal::getInstance().sendDelay = delay;
}

void BluetoothTerminal::start()
{
  Serial.print("[BluetoothTerminal] Starting BLE service...");
  if (!BLE.begin())
  {
    Serial.println(" failed!");
    return;
  }
  Serial.println(" successful.");

  Serial.println("[BluetoothTerminal] Setting up BLE service and characteristic...");

  BluetoothTerminal instance = BluetoothTerminal::getInstance();

  if (instance.name != nullptr)
  {
    BLE.setLocalName(instance.name);
    BLE.setDeviceName(instance.name);
  }

  BLE.setEventHandler(BLEConnected, BluetoothTerminal::handleConnectedStatic);
  BLE.setEventHandler(BLEDisconnected, BluetoothTerminal::handleDisconnectedStatic);

  instance.bleService.addCharacteristic(instance.bleCharacteristic);
  instance.bleCharacteristic.setEventHandler(BLEWritten, BluetoothTerminal::handleWrittenStatic);
  BLE.addService(instance.bleService);

  BLE.setAdvertisedService(instance.bleService);
  BLE.advertise();

  Serial.println("[BluetoothTerminal] BLE service and characteristic were set up.");
}

void BluetoothTerminal::loop()
{
  BLE.poll();
}

bool BluetoothTerminal::isConnected()
{
  return BluetoothTerminal::getInstance().deviceConnected;
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
  int lengthWithSeparator = length + 1;

  // Send the message if its length including the send separator is within the
  // characteristic value size.
  if (lengthWithSeparator <= instance.characteristicValueSize)
  {
    // An additional byte accommodates the null terminator. A string longer
    // than the characteristic value size can be created, however, no problems
    // were observed during testing.
    char messageWithSeparator[lengthWithSeparator + 1];
    strcpy(messageWithSeparator, message);
    messageWithSeparator[lengthWithSeparator - 1] = instance.sendSeparator;
    messageWithSeparator[lengthWithSeparator] = '\0';

    instance.bleCharacteristic.setValue(messageWithSeparator);

    Serial.println(" sent.");
    return;
  }

  // Otherwise, split the message into chunks and send them one by one with a
  // small delay.
  int numOfChunks = lengthWithSeparator / instance.characteristicValueSize;
  if (lengthWithSeparator % instance.characteristicValueSize > 0)
  {
    numOfChunks += 1;
  }

  Serial.println();
  Serial.print("[BluetoothTerminal]   Message length (");
  Serial.print(length);
  Serial.print(" characters) with the separator is longer than the characteristic value size (");
  Serial.print(instance.characteristicValueSize);
  Serial.print(" bytes), sending in ");
  Serial.print(numOfChunks);
  Serial.println(" chunks...");

  for (int i = 0; i < numOfChunks; i++)
  {
    int offset = i * instance.characteristicValueSize;
    int chunkLength = (i == numOfChunks - 1)
                          ? lengthWithSeparator - offset
                          : instance.characteristicValueSize;

    // An additional byte accommodates the null terminator.
    char chunk[chunkLength + 1];
    strncpy(chunk, message + offset, chunkLength);
    // Add a send separator if it's the last chunk.
    if (i == numOfChunks - 1)
    {
      chunk[chunkLength - 1] = instance.sendSeparator;
    }
    chunk[chunkLength] = '\0';

    Serial.print("[BluetoothTerminal]     Sending chunk ");
    Serial.print(i + 1);
    Serial.print(": \"");
    Serial.print(chunk);
    Serial.print("\"...");

    instance.bleCharacteristic.setValue(chunk);

    if (i < numOfChunks - 1 && instance.sendDelay > 0)
    {
      delay(instance.sendDelay);
    }

    Serial.println(" sent.");
  }

  Serial.println("[BluetoothTerminal]   All message chunks were sent.");
}

void BluetoothTerminal::handleConnectedStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().handleConnected(device);
}

void BluetoothTerminal::handleDisconnectedStatic(BLEDevice device)
{
  BluetoothTerminal::getInstance().handleDisconnected(device);
}

void BluetoothTerminal::handleWrittenStatic(BLEDevice device, BLECharacteristic characteristic)
{
  BluetoothTerminal::getInstance().handleWritten(device, characteristic);
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::handleConnected(BLEDevice device)
{
  this->deviceConnected = true;

  Serial.print("[BluetoothTerminal] Device (address \"");
  Serial.print(device.address());
  Serial.println("\") was connected.");

  if (this->connectHandler)
  {
    this->connectHandler(device);
  }
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::handleDisconnected(BLEDevice device)
{
  this->deviceConnected = false;

  Serial.print("[BluetoothTerminal] Device (address \"");
  Serial.print(device.address());
  Serial.println("\") was disconnected.");

  if (this->disconnectHandler)
  {
    this->disconnectHandler(device);
  }
}

/**
 * This method is called from the static method, so `this` references the
 * singleton instance.
 */
void BluetoothTerminal::handleWritten(BLEDevice device, BLECharacteristic characteristic)
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

      Serial.print("[BluetoothTerminal]   Message received: \"");
      Serial.print(this->receiveBuffer);
      Serial.println("\".");

      if (this->receiveHandler)
      {
        this->receiveHandler(this->receiveBuffer);
      }

      this->receiveBufferIndex = 0;

      continue;
    }

    // If the separator was not received and the receive buffer cannot
    // accommodate additional characters, discard data in the buffer and reset.
    if (this->receiveBufferIndex > this->receiveBufferSize - 2)
    {
      this->receiveBuffer[this->receiveBufferIndex] = '\0';

      Serial.print("[BluetoothTerminal]   Receive buffer overflow, data discarded: \"");
      Serial.print(this->receiveBuffer);
      Serial.println("\".");

      this->receiveBufferIndex = 0;
    }

    this->receiveBuffer[this->receiveBufferIndex++] = _char;
  }
}
