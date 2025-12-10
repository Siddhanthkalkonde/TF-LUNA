/*
 * TF-Luna UART Raw Data Access
 * Direct byte-level manipulation
 */

#include <TF_LUNA_LIDAR.h>

TF_LUNA_LIDAR lidar(18, 19, 115200);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  lidar.begin();
  
  Serial.println("╔══════════════════════════════════════╗");
  Serial.println("║     TF-Luna Raw Data Example        ║");
  Serial.println("╚══════════════════════════════════════╝");
  
  Serial.println("Raw frame format: [0x59 0x59 Dist_L Dist_H Str_L Str_H Temp_L Temp_H ChkSum]");
  Serial.println("========================================");
}

void loop() {
  if (lidar.readRawData()) {
    uint8_t* raw = lidar.getRawBuffer();
    
    // Print raw bytes
    Serial.print("Raw: ");
    for (int i = 0; i < 9; i++) {
      Serial.print("0x");
      if (raw[i] < 0x10) Serial.print("0");
      Serial.print(raw[i], HEX);
      Serial.print(" ");
    }
    
    // Print decoded values
    Serial.print(" | Decoded: ");
    Serial.print(lidar.getDistance());
    Serial.print("cm ");
    Serial.print(lidar.getStrength());
    Serial.print("str ");
    Serial.print(lidar.getTemperature(), 1);
    Serial.println("°C");
    
    // Alternative manual decoding
    uint16_t manualDist = raw[2] | (raw[3] << 8);
    uint16_t manualStr = raw[4] | (raw[5] << 8);
    
    Serial.print("Manual: ");
    Serial.print(manualDist);
    Serial.print("cm ");
    Serial.print(manualStr);
    Serial.println("str");
    
    Serial.println("----------------------------------------");
  }
  
  delay(500);
}
