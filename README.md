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
// UART Mode
#include <TF_LUNA_LIDAR.h>
TF_LUNA_LIDAR lidar(18, 19, 115200); // RX, TX, baud

void setup() {
  lidar.begin();
}

void loop() {
  if (lidar.readData()) {
    Serial.println(lidar.getDistance());
  }
}

// I2C Mode
TF_LUNA_LIDAR lidar(0x10, 21, 22, true); // addr, SDA, SCL, I2C mode
License
MIT
# TF-LUNA
