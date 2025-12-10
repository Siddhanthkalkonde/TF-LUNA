/*
 * TF-Luna I2C Basic Example
 * ESP32 Pins: SDA=GPIO21, SCL=GPIO22
 * I2C Address: 0x10
 */

#include <TF_LUNA_LIDAR.h>

// I2C Mode: (address, sdaPin, sclPin, true_for_I2C)
TF_LUNA_LIDAR lidar(0x10, 21, 22, true);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔══════════════════════════════════════╗");
  Serial.println("║     TF-Luna I2C Basic Example       ║");
  Serial.println("╚══════════════════════════════════════╝");
  
  if (!lidar.begin()) {
    Serial.println("TF-Luna not found on I2C bus!");
    lidar.scanI2C();
    while(1);
  }
  
  Serial.println("TF-Luna found at address 0x10");
  Serial.println("========================================");
}

void loop() {
  if (lidar.readDataI2C()) {
    Serial.print("I2C Distance: ");
    Serial.print(lidar.getDistance());
    Serial.print(" cm  |  Strength: ");
    Serial.print(lidar.getStrength());
    Serial.print("  |  Temp: ");
    Serial.print(lidar.getTemperature(), 1);
    Serial.println(" °C");
  } else {
    Serial.println("I2C read failed");
  }
  
  delay(100);
}
