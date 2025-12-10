#ifndef TF_LUNA_LIDAR_H
#define TF_LUNA_LIDAR_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>

class TF_LUNA_LIDAR {
  public:
    // === CONSTRUCTORS (UART Mode) ===
    // Use explicit uint32_t for baud to avoid ambiguity
    TF_LUNA_LIDAR(uint8_t rxPin, uint8_t txPin, uint32_t baudRate);
    
    // === CONSTRUCTORS (I2C Mode) ===
    // I2C constructor with different parameter order
    TF_LUNA_LIDAR(uint8_t i2cAddress, uint8_t sdaPin, uint8_t sclPin, bool i2cMode);
    
    // === INITIALIZATION ===
    bool begin();
    
    // === DATA READING ===
    bool readData();           // Normal read
    bool readRawData();        // Raw data for speed
    bool readDataI2C();        // I2C specific
    
    // === GETTERS ===
    uint16_t getDistance() { return distance; }
    uint16_t getStrength() { return strength; }
    float getTemperature() { return temperature; }
    uint8_t* getRawBuffer() { return rawBuffer; }
    
    // === MODE CONTROL ===
    void setMode(bool i2cMode) { isI2C = i2cMode; }
    bool isI2CMode() { return isI2C; }
    bool isUARTMode() { return !isI2C; }
    
    // === UTILITIES ===
    void scanI2C();            // Scan I2C bus
    void sendCommandUART(uint8_t* cmd, uint8_t len);
    void configureUART();
    void runDiagnostics();
    
    // === SPEED OPTIMIZATION ===
    void enableFastMode(bool enable);
    uint32_t getFrameCount() { return frameCount; }
    float getFPS();
    
  private:
    // Mode
    bool isI2C;
    
    // UART data
    uint8_t rxPin, txPin;
    uint32_t baudRate;
    HardwareSerial* tfSerial;
    
    // I2C data
    uint8_t i2cAddr, sdaPin, sclPin;
    
    // Measurement data
    uint16_t distance, strength;
    float temperature;
    
    // Raw buffer for fast access
    uint8_t rawBuffer[9];
    
    // Performance tracking
    uint32_t frameCount;
    uint32_t lastFpsTime;
    uint32_t fpsCounter;
    bool fastMode;
    
    // Private methods
    bool verifyChecksum(uint8_t* data);
    bool readFrameUART();
    bool readFrameI2C();
};

#endif // TF_LUNA_LIDAR_H
