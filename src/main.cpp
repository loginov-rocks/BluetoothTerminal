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

  bluetoothTerminal.onConnect(handleConnect);
  bluetoothTerminal.onDisconnect(handleDisconnect);
  bluetoothTerminal.onReceive(handleReceive);
  // bluetoothTerminal.setReceiveSeparator('\n');
  // bluetoothTerminal.setSendSeparator('\n');
  bluetoothTerminal.start();

  Serial.println("Bluetooth Terminal setup was successful.");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis > lastMillis + 10000)
  {
    lastMillis = currentMillis;

    if (bluetoothTerminal.isConnected())
    {
      bluetoothTerminal.send("Hello, world!");
    }
  }

  bluetoothTerminal.loop();
}
