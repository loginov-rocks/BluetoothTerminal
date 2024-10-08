#include <Arduino.h>

#include <BluetoothTerminal.h>

BluetoothTerminal bluetoothTerminal;
unsigned long lastMillis = 0;

void handleConnect(BLEDevice device)
{
  Serial.println("Device connected");
}

void handleDisconnect(BLEDevice device)
{
  Serial.println("Device disconnected");
}

void handleReceive(const char *message)
{
  Serial.print("Message received: ");
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);

  Serial.println("Setting up Bluetooth Terminal...");

  bluetoothTerminal.enableDebug(); // optional

  bluetoothTerminal.onConnect(handleConnect);       // optional
  bluetoothTerminal.onDisconnect(handleDisconnect); // optional
  bluetoothTerminal.onReceive(handleReceive);       // optional

  // bluetoothTerminal.setServiceUuid("ffe0");
  // bluetoothTerminal.setCharacteristicUuid("ffe1");
  // bluetoothTerminal.setCharacteristicValueSize(20);

  bluetoothTerminal.setName("BluetoothTerminal"); // optional
  // bluetoothTerminal.setReceiveBufferSize(256);
  // bluetoothTerminal.setReceiveSeparator('\n');
  // bluetoothTerminal.setSendSeparator('\n');
  // bluetoothTerminal.setSendDelay(100);

  bluetoothTerminal.start();

  Serial.println("Bluetooth Terminal setup was successful.");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis > lastMillis + 5000)
  {
    lastMillis = currentMillis;

    if (bluetoothTerminal.isConnected())
    {
      bluetoothTerminal.send("Hello, world!");
    }
  }

  bluetoothTerminal.loop();
}
