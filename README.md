# TF-LUNA-LIDAR Complete Library

Complete ESP32 library for Benewake TF-Luna LiDAR sensor with UART and I2C support.

## Features
- **UART Basic**: Normal operation with diagnostics
- **UART Fast**: Maximum speed with raw data
- **UART Raw**: Byte-level frame access
- **I2C Basic**: I2C communication support
- **Performance tracking**: FPS calculation
- **Diagnostics**: Connection testing

## Examples
1. **UART_Basic** - Normal operation with formatted output
2. **UART_Fast** - Maximum speed for real-time applications
3. **UART_Raw** - Direct byte access for debugging
4. **I2C_Basic** - I2C communication example

## Wiring
### UART Mode:
TF-Luna → ESP32
TX (Green) → GPIO18 (RX)
RX (White) → GPIO19 (TX)
VCC (Red) → 3.3V or 5V
GND (Black) → GND

text

### I2C Mode:
TF-Luna → ESP32
SDA (Blue) → GPIO21 (SDA)
SCL (Brown) → GPIO22 (SCL)
VCC (Red) → 3.3V
GND (Black) → GND

text

## Quick Start
```cpp
/*
 * TF-Luna UART Basic Example
 * ESP32 Pins: RX=GPIO18, TX=GPIO19
 */

#include <TF_LUNA_LIDAR.h>

// UART Mode: (rxPin, txPin, baudRate)
TF_LUNA_LIDAR lidar(18, 19, 115200);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔══════════════════════════════════════╗");
  Serial.println("║     TF-Luna UART Basic Example      ║");
  Serial.println("╚══════════════════════════════════════╝");
  
  if (!lidar.begin()) {
    Serial.println("Failed to initialize!");
    while(1);
  }
  
  // Optional configuration
  lidar.configureUART();
  
  Serial.println("Ready. Place object in front of TF-Luna.");
  Serial.println("========================================");
}

void loop() {
  if (lidar.readData()) {
    Serial.print("Distance: ");
    Serial.print(lidar.getDistance());
    Serial.print(" cm  |  Strength: ");
    Serial.print(lidar.getStrength());
    
    // Signal quality indicator
    uint16_t str = lidar.getStrength();
    Serial.print("  [");
    if (str < 100) Serial.print("Poor");
    else if (str < 300) Serial.print("Fair");
    else if (str < 700) Serial.print("Good");
    else Serial.print("Excellent");
    Serial.print("]");
    
    Serial.print("  |  Temp: ");
    Serial.print(lidar.getTemperature(), 1);
    Serial.println(" °C");
  } else {
    static uint32_t lastMsg = 0;
    if (millis() - lastMsg > 2000) {
      Serial.println("No data...");
      lastMsg = millis();
    }
  }
  
  delay(10);
}

'''
