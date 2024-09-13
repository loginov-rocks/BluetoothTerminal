#include <BluetoothTerminal.h>

BluetoothTerminal::~BluetoothTerminal()
{
  if (this->bleService != nullptr)
  {
    delete this->bleService;
  }

  if (this->bleCharacteristic != nullptr)
  {
    delete this->bleCharacteristic;
  }

  if (this->receiveBuffer != nullptr)
  {
    delete[] this->receiveBuffer;
  }

  if (this->serviceUuid != nullptr)
  {
    delete[] this->serviceUuid;
  }

  if (this->characteristicUuid != nullptr)
  {
    delete[] this->characteristicUuid;
  }

  if (this->name != nullptr)
  {
    delete[] this->name;
  }
}

void BluetoothTerminal::onConnect(ConnectHandler handler)
{
  BluetoothTerminal::getInstance().connectHandler = handler;

  Serial.println("[BluetoothTerminal] The connect handler is set.");
}

void BluetoothTerminal::onDisconnect(DisconnectHandler handler)
{
  BluetoothTerminal::getInstance().disconnectHandler = handler;

  Serial.println("[BluetoothTerminal] The disconnect handler is set.");
}

void BluetoothTerminal::onReceive(ReceiveHandler handler)
{
  BluetoothTerminal::getInstance().receiveHandler = handler;

  Serial.println("[BluetoothTerminal] The receive handler is set.");
}

bool BluetoothTerminal::setServiceUuid(const char *uuid)
{
  if (uuid == nullptr)
  {
    return false;
  }

  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  if (instance.serviceUuid != nullptr)
  {
    Serial.print("[BluetoothTerminal] Setting the service UUID failed, it was already set to \"");
    Serial.print(instance.serviceUuid);
    Serial.println("\"!");

    return false;
  }

  size_t length = strlen(uuid) + 1;
  instance.serviceUuid = new char[length];
  strncpy(instance.serviceUuid, uuid, length);

  Serial.print("[BluetoothTerminal] The service UUID is set to \"");
  Serial.print(instance.serviceUuid);
  Serial.println("\".");

  return true;
}

bool BluetoothTerminal::setCharacteristicUuid(const char *uuid)
{
  if (uuid == nullptr)
  {
    return false;
  }

  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  if (instance.characteristicUuid != nullptr)
  {
    Serial.print("[BluetoothTerminal] Setting the characteristic UUID failed, it was already set to \"");
    Serial.print(instance.characteristicUuid);
    Serial.println("\"!");

    return false;
  }

  size_t length = strlen(uuid) + 1;
  instance.characteristicUuid = new char[length];
  strncpy(instance.characteristicUuid, uuid, length);

  Serial.print("[BluetoothTerminal] The characteristic UUID is set to \"");
  Serial.print(instance.characteristicUuid);
  Serial.println("\".");

  return true;
}

void BluetoothTerminal::setCharacteristicValueSize(int size)
{
  BluetoothTerminal::getInstance().characteristicValueSize = size;

  Serial.print("[BluetoothTerminal] The characteristic value size is set to ");
  Serial.print(size);
  Serial.println(" bytes.");
}

bool BluetoothTerminal::setName(const char *name)
{
  if (name != nullptr)
  {
    return false;
  }

  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  if (instance.name != nullptr)
  {
    Serial.print("[BluetoothTerminal] Setting the name failed, it was already set to \"");
    Serial.print(instance.name);
    Serial.println("\"!");

    return false;
  }

  size_t length = strlen(name) + 1;
  instance.name = new char[length];
  strncpy(instance.name, name, length);

  Serial.print("[BluetoothTerminal] The name is set to \"");
  Serial.print(instance.name);
  Serial.println("\".");

  return true;
}

void BluetoothTerminal::setReceiveBufferSize(size_t size)
{
  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  // Set the new receive buffer size only if it's different from the current
  // one.
  if (size != instance.receiveBufferSize)
  {
    // Delete the current receive buffer to reallocate memory after.
    if (instance.receiveBuffer != nullptr)
    {
      delete[] instance.receiveBuffer;
    }

    instance.receiveBufferInitialized = false;
    instance.receiveBufferSize = size;

    Serial.print("[BluetoothTerminal] The receive buffer size is set to ");
    Serial.print(instance.receiveBufferSize);
    Serial.println(" bytes.");
  }
}

void BluetoothTerminal::setReceiveSeparator(char separator)
{
  BluetoothTerminal::getInstance().receiveSeparator = separator;

  Serial.print("[BluetoothTerminal] The receive separator is set to \"");
  Serial.print(separator);
  Serial.println("\".");
}

void BluetoothTerminal::setSendSeparator(char separator)
{
  BluetoothTerminal::getInstance().sendSeparator = separator;

  Serial.print("[BluetoothTerminal] The send separator is set to \"");
  Serial.print(separator);
  Serial.println("\".");
}

void BluetoothTerminal::setSendDelay(int delay)
{
  BluetoothTerminal::getInstance().sendDelay = delay;

  Serial.print("[BluetoothTerminal] The send delay is set to ");
  Serial.print(delay);
  Serial.println(" milliseconds.");
}

bool BluetoothTerminal::start()
{
  Serial.print("[BluetoothTerminal] Starting BLE service...");

  if (!BLE.begin())
  {
    Serial.println(" failed!");

    return false;
  }

  Serial.println(" successful.");

  Serial.println("[BluetoothTerminal] Setting up BLE service and characteristic...");

  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  if (instance.name != nullptr)
  {
    BLE.setLocalName(instance.name);
    BLE.setDeviceName(instance.name);
  }

  BLE.setEventHandler(BLEConnected, BluetoothTerminal::handleConnectedStatic);
  BLE.setEventHandler(BLEDisconnected, BluetoothTerminal::handleDisconnectedStatic);

  if (instance.bleService != nullptr)
  {
    delete instance.bleService;
  }

  if (instance.bleCharacteristic != nullptr)
  {
    delete instance.bleCharacteristic;
  }

  instance.bleService = new BLEService(instance.serviceUuid == nullptr ? "ffe0" : instance.serviceUuid);
  instance.bleCharacteristic = new BLECharacteristic(
      instance.characteristicUuid == nullptr ? "ffe1" : instance.characteristicUuid,
      BLERead | BLEWriteWithoutResponse | BLENotify,
      instance.characteristicValueSize);

  instance.bleService->addCharacteristic(*instance.bleCharacteristic);
  instance.bleCharacteristic->setEventHandler(BLEWritten, BluetoothTerminal::handleWrittenStatic);
  BLE.addService(*instance.bleService);

  BLE.setAdvertisedService(*instance.bleService);
  BLE.advertise();

  Serial.println("[BluetoothTerminal] BLE service and characteristic were set up.");

  return true;
}

void BluetoothTerminal::loop()
{
  BLE.poll();
}

bool BluetoothTerminal::isConnected()
{
  return BluetoothTerminal::getInstance().deviceConnected;
}

bool BluetoothTerminal::send(const char *message)
{
  Serial.print("[BluetoothTerminal] Sending message: \"");
  Serial.print(message);
  Serial.print("\"...");

  BluetoothTerminal &instance = BluetoothTerminal::getInstance();

  if (!instance.isConnected())
  {
    Serial.println(" failed, no device connected!");

    return false;
  }

  size_t length = strlen(message);
  size_t lengthWithSeparator = length + 1;

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

    instance.bleCharacteristic->setValue(messageWithSeparator);

    Serial.println(" sent.");

    return true;
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
    size_t chunkLength = (i == numOfChunks - 1)
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

    instance.bleCharacteristic->setValue(chunk);

    if (i < numOfChunks - 1 && instance.sendDelay > 0)
    {
      delay(instance.sendDelay);
    }

    Serial.println(" sent.");
  }

  Serial.println("[BluetoothTerminal]   All message chunks were sent.");

  return true;
}

BluetoothTerminal &BluetoothTerminal::getInstance()
{
  // This instance will be created once and reused.
  static BluetoothTerminal instance;

  return instance;
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

  // Dynamically allocate memory for the receive buffer, if it's not
  // initialized yet.
  if (!this->receiveBufferInitialized)
  {
    this->receiveBuffer = new char[this->receiveBufferSize];
    this->receiveBufferInitialized = true;
  }

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
