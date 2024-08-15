#include <Arduino.h>

#include <ArduinoBLE.h>

const char serviceUuid[] = "ffe0";
const char characteristicUuid[] = "ffe1";

BLEService myService(serviceUuid);
BLECharacteristic myCharacteristic(characteristicUuid, BLERead | BLEWriteWithoutResponse | BLENotify, 20);

bool deviceConnected = false;
char buffer[128];
int bufferI = 0;
unsigned long lastMillis = 0;

void onConnect(BLEDevice device)
{
  deviceConnected = true;
  Serial.println("Device connected!");
}

void onDisconnect(BLEDevice device)
{
  deviceConnected = false;
  Serial.println("Device disconnected!");
}

void onWrite(BLEDevice device, BLECharacteristic characteristic)
{
  Serial.println("Central device has written to the characteristic!");

  const uint8_t *data = characteristic.value();
  int length = characteristic.valueLength();

  for (int i = 0; i < length; i++)
  {
    const char _char = (char)data[i];
    buffer[bufferI++] = _char;

    if (_char == '\n')
    {
      buffer[bufferI] = '\0';
      Serial.print("Received message: \"");
      Serial.print(buffer);
      Serial.println("\"");

      bufferI = 0;
    }
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.println("Set up BLE...");

  if (!BLE.begin())
  {
    Serial.println("Failed!");
    while (true)
      ;
  }

  BLE.setLocalName("ESP32");
  BLE.setDeviceName("ESP32");

  BLE.setEventHandler(BLEConnected, onConnect);
  BLE.setEventHandler(BLEDisconnected, onDisconnect);

  myService.addCharacteristic(myCharacteristic);
  myCharacteristic.setEventHandler(BLEWritten, onWrite);
  BLE.addService(myService);

  BLE.setAdvertisedService(myService);
  BLE.advertise();

  Serial.println("BLE setup was successful!");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis > lastMillis + 1000)
  {
    lastMillis = currentMillis;

    if (deviceConnected)
    {
      myCharacteristic.setValue("Test\n");
      Serial.println("Device is connected, idle...");
    }
  }

  BLE.poll();
}
