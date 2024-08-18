#include <Arduino.h>

#include <BluetoothTerminal.h>

BluetoothTerminal bluetoothTerminal;
unsigned long lastMillis = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println("Setting up Bluetooth Terminal...");
  bluetoothTerminal.setup();
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
