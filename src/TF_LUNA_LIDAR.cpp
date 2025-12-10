#include "TF_LUNA_LIDAR.h"

// === UART CONSTRUCTOR ===
TF_LUNA_LIDAR::TF_LUNA_LIDAR(uint8_t rxPin, uint8_t txPin, uint32_t baudRate) {
    this->isI2C = false;
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->baudRate = baudRate;
    this->tfSerial = new HardwareSerial(2);
    
    distance = 0;
    strength = 0;
    temperature = 0.0;
    frameCount = 0;
    lastFpsTime = 0;
    fpsCounter = 0;
    fastMode = false;
}

// === I2C CONSTRUCTOR ===
TF_LUNA_LIDAR::TF_LUNA_LIDAR(uint8_t i2cAddress, uint8_t sdaPin, uint8_t sclPin, bool i2cMode) {
    this->isI2C = true;
    this->i2cAddr = i2cAddress;
    this->sdaPin = sdaPin;
    this->sclPin = sclPin;
    this->tfSerial = nullptr;
    
    distance = 0;
    strength = 0;
    temperature = 0.0;
    frameCount = 0;
}

// === INITIALIZATION ===
bool TF_LUNA_LIDAR::begin() {
    if (isI2C) {
        // I2C initialization
        Wire.begin(sdaPin, sclPin);
        Wire.setClock(100000);
        
        // Check if device responds
        Wire.beginTransmission(i2cAddr);
        return (Wire.endTransmission() == 0);
    } else {
        // UART initialization
        tfSerial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
        
        // Clear buffer
        while(tfSerial->available()) tfSerial->read();
        return true;
    }
}

// === READ DATA (Universal) ===
bool TF_LUNA_LIDAR::readData() {
    if (isI2C) {
        return readDataI2C();
    } else {
        return readFrameUART();
    }
}

// === READ RAW DATA (Fast UART) ===
bool TF_LUNA_LIDAR::readRawData() {
    if (isI2C || !tfSerial) return false;
    
    if (tfSerial->available() >= 9) {
        // Read 9 bytes directly
        tfSerial->readBytes(rawBuffer, 9);
        
        // Quick header check
        if (rawBuffer[0] == 0x59 && rawBuffer[1] == 0x59) {
            // Verify checksum
            if (verifyChecksum(rawBuffer)) {
                // Extract data
                distance = rawBuffer[2] + (rawBuffer[3] << 8);
                strength = rawBuffer[4] + (rawBuffer[5] << 8);
                uint16_t tempRaw = rawBuffer[6] + (rawBuffer[7] << 8);
                temperature = (tempRaw / 8.0) - 256.0;
                frameCount++;
                return true;
            }
        }
    }
    return false;
}

// === READ FRAME UART ===
bool TF_LUNA_LIDAR::readFrameUART() {
    if (!tfSerial) return false;
    
    if (fastMode) {
        return readRawData();
    }
    
    if (tfSerial->available() >= 9) {
        uint8_t buffer[9];
        
        // Look for header
        bool foundHeader = false;
        while (tfSerial->available() >= 2 && !foundHeader) {
            uint8_t b1 = tfSerial->read();
            if (b1 == 0x59 && tfSerial->peek() == 0x59) {
                buffer[0] = b1;
                buffer[1] = tfSerial->read();
                foundHeader = true;
            }
        }
        
        if (!foundHeader) return false;
        
        // Read remaining bytes
        for (int i = 2; i < 9; i++) {
            unsigned long start = millis();
            while (!tfSerial->available()) {
                if (millis() - start > 10) return false;
            }
            buffer[i] = tfSerial->read();
        }
        
        // Verify checksum
        if (verifyChecksum(buffer)) {
            // Copy to raw buffer
            memcpy(rawBuffer, buffer, 9);
            
            // Extract data
            distance = buffer[2] + (buffer[3] << 8);
            strength = buffer[4] + (buffer[5] << 8);
            uint16_t tempRaw = buffer[6] + (buffer[7] << 8);
            temperature = (tempRaw / 8.0) - 256.0;
            frameCount++;
            return true;
        }
    }
    return false;
}

// === READ DATA I2C ===
bool TF_LUNA_LIDAR::readDataI2C() {
    if (!isI2C) return false;
    
    // Trigger measurement
    uint8_t triggerCmd[] = {0x5A, 0x05, 0x00, 0x01, 0x60};
    Wire.beginTransmission(i2cAddr);
    Wire.write(triggerCmd, 5);
    Wire.endTransmission();
    
    delay(10);
    
    // Read response
    Wire.requestFrom(i2cAddr, (uint8_t)9);
    if (Wire.available() >= 9) {
        uint8_t buffer[9];
        for (int i = 0; i < 9; i++) {
            buffer[i] = Wire.read();
        }
        
        if (buffer[0] == 0x59 && buffer[1] == 0x59 && verifyChecksum(buffer)) {
            memcpy(rawBuffer, buffer, 9);
            distance = buffer[2] + (buffer[3] << 8);
            strength = buffer[4] + (buffer[5] << 8);
            uint16_t tempRaw = buffer[6] + (buffer[7] << 8);
            temperature = (tempRaw / 8.0) - 256.0;
            frameCount++;
            return true;
        }
    }
    return false;
}

// === VERIFY CHECKSUM ===
bool TF_LUNA_LIDAR::verifyChecksum(uint8_t* data) {
    uint8_t sum = 0;
    for (int i = 0; i < 8; i++) sum += data[i];
    return (sum == data[8]);
}

// === SCAN I2C ===
void TF_LUNA_LIDAR::scanI2C() {
    if (!isI2C) return;
    
    Serial.println("Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("Found device at 0x");
            if (addr < 16) Serial.print("0");
            Serial.println(addr, HEX);
        }
    }
}

// === SEND UART COMMAND ===
void TF_LUNA_LIDAR::sendCommandUART(uint8_t* cmd, uint8_t len) {
    if (isI2C || !tfSerial) return;
    
    for (int i = 0; i < len; i++) {
        tfSerial->write(cmd[i]);
    }
    delay(10);
}

// === CONFIGURE UART ===
void TF_LUNA_LIDAR::configureUART() {
    if (isI2C) return;
    
    // Enable output
    uint8_t enableCmd[] = {0x5A, 0x04, 0x04, 0x62};
    sendCommandUART(enableCmd, 4);
    
    // Save settings
    uint8_t saveCmd[] = {0x5A, 0x04, 0x11, 0x6F};
    sendCommandUART(saveCmd, 4);
    
    Serial.println("TF-Luna configured for UART");
}

// === ENABLE FAST MODE ===
void TF_LUNA_LIDAR::enableFastMode(bool enable) {
    fastMode = enable;
    if (tfSerial) {
        tfSerial->setRxBufferSize(enable ? 256 : 128);
    }
}

// === GET FPS ===
float TF_LUNA_LIDAR::getFPS() {
    uint32_t now = millis();
    uint32_t elapsed = now - lastFpsTime;
    
    if (elapsed >= 1000) {
        float fps = (fpsCounter * 1000.0) / elapsed;
        lastFpsTime = now;
        fpsCounter = 0;
        return fps;
    }
    fpsCounter++;
    return -1;
}

// === RUN DIAGNOSTICS ===
void TF_LUNA_LIDAR::runDiagnostics() {
    if (isI2C) {
        scanI2C();
    } else {
        Serial.println("UART Diagnostics:");
        Serial.println("Listening for 3 seconds...");
        
        unsigned long start = millis();
        int byteCount = 0;
        
        while (millis() - start < 3000) {
            if (tfSerial->available()) {
                uint8_t data = tfSerial->read();
                Serial.print("0x");
                if (data < 0x10) Serial.print("0");
                Serial.print(data, HEX);
                Serial.print(" ");
                byteCount++;
                if (byteCount % 16 == 0) Serial.println();
            }
        }
        
        Serial.println("\nTotal bytes: " + String(byteCount));
    }
}
