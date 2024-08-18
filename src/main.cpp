#include <Arduino.h>

#include <BluetoothTerminal.h>

BluetoothTerminal bluetoothTerminal;
unsigned long lastMillis = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println("Setting up Bluetooth Terminal...");
  bluetoothTerminal.begin();
  Serial.println("Bluetooth Terminal setup was successful!");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis > lastMillis + 1000)
  {
    lastMillis = currentMillis;

    if (bluetoothTerminal.isDeviceConnected())
    {
      bluetoothTerminal.send("Test\n");
    }
  }

  bluetoothTerminal.loop();
}
