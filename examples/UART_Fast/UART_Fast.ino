/*
 * TF-Luna UART Fast Mode
 * Maximum speed with raw data access
 */

#include <TF_LUNA_LIDAR.h>

TF_LUNA_LIDAR lidar(18, 19, 115200);
uint32_t lastPrint = 0;
float fps = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  
  lidar.begin();
  lidar.enableFastMode(true);  // Enable fast mode
  
  Serial.println("TF-Luna Fast Mode");
  Serial.println("Format: Distance,Strength,FPS");
  Serial.println("----------------------------");
}

void loop() {
  // Read as fast as possible
  if (lidar.readRawData()) {
    // Get FPS every second
    float currentFps = lidar.getFPS();
    if (currentFps > 0) {
      fps = currentFps;
    }
    
    // Print at 10Hz
    if (millis() - lastPrint > 100) {
      Serial.print(lidar.getDistance());
      Serial.print(" cm	,");
      Serial.print(lidar.getStrength());
      Serial.print("	,");
      Serial.println(fps, 1);
      lastPrint = millis();
    }
  }
  
  // No delay for maximum speed
}
