#pragma once

#include <Arduino.h>
#include <SPI.h>

// TMC5130A Register Addresses
#define TMC5130A_GCONF      0x00
#define TMC5130A_GSTAT      0x01
#define TMC5130A_IHOLD_IRUN 0x10
#define TMC5130A_RAMPMODE   0x20
#define TMC5130A_XACTUAL    0x21
#define TMC5130A_VACTUAL    0x22
#define TMC5130A_VSTART     0x23
#define TMC5130A_A1         0x24
#define TMC5130A_V1         0x25
#define TMC5130A_AMAX       0x26
#define TMC5130A_VMAX       0x27
#define TMC5130A_DMAX       0x28
#define TMC5130A_D1         0x2A
#define TMC5130A_VSTOP      0x2B
#define TMC5130A_TZEROWAIT  0x2C
#define TMC5130A_XTARGET    0x2D

class StepperController {
public:
    StepperController(uint8_t cs_pin, uint8_t en_pin, uint32_t max_speed = 200000) 
        : cs_pin_(cs_pin), en_pin_(en_pin), max_speed_(max_speed) {}

    void begin() {
        // Configure pins
        pinMode(cs_pin_, OUTPUT);
        pinMode(en_pin_, OUTPUT);
        digitalWrite(cs_pin_, HIGH);  // CS is active low
        digitalWrite(en_pin_, HIGH);  // Enable is active low

        // Initialize SPI
        SPI.begin();
        
        // Configure TMC5130A
        writeRegister(TMC5130A_GCONF, 0x00000004);      // Enable internal voltage regulator
        writeRegister(TMC5130A_IHOLD_IRUN, 0x00071703); // Set motor current
        
        // Configure ramp parameters
        writeRegister(TMC5130A_RAMPMODE, 0);            // Position mode
        writeRegister(TMC5130A_VSTART, 0);              // Start velocity
        writeRegister(TMC5130A_A1, 1000);              // First acceleration
        writeRegister(TMC5130A_V1, 50000);             // First velocity
        writeRegister(TMC5130A_AMAX, 5000);            // Max acceleration
        writeRegister(TMC5130A_VMAX, max_speed_);      // Max velocity
        writeRegister(TMC5130A_DMAX, 5000);            // Max deceleration
        writeRegister(TMC5130A_D1, 1000);              // First deceleration
        writeRegister(TMC5130A_VSTOP, 10);             // Stop velocity
    }

    void enable() {
        digitalWrite(en_pin_, LOW);
    }

    void disable() {
        digitalWrite(en_pin_, HIGH);
    }

    void setSpeed(uint32_t speed) {
        if (speed > max_speed_) speed = max_speed_;
        writeRegister(TMC5130A_VMAX, speed);
    }

    void setPosition(int32_t position) {
        writeRegister(TMC5130A_XTARGET, position);
    }

    int32_t getCurrentPosition() {
        return readRegister(TMC5130A_XACTUAL);
    }

    int32_t getCurrentVelocity() {
        return readRegister(TMC5130A_VACTUAL);
    }

    void stop() {
        writeRegister(TMC5130A_RAMPMODE, 1);  // Velocity mode
        writeRegister(TMC5130A_VMAX, 0);      // Target velocity = 0
    }

private:
    uint8_t cs_pin_;
    uint8_t en_pin_;
    uint32_t max_speed_;

    void writeRegister(uint8_t addr, uint32_t data) {
        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
        digitalWrite(cs_pin_, LOW);
        
        SPI.transfer(addr | 0x80);  // Set write bit
        SPI.transfer((data >> 24) & 0xFF);
        SPI.transfer((data >> 16) & 0xFF);
        SPI.transfer((data >> 8) & 0xFF);
        SPI.transfer(data & 0xFF);
        
        digitalWrite(cs_pin_, HIGH);
        SPI.endTransaction();
    }

    uint32_t readRegister(uint8_t addr) {
        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
        digitalWrite(cs_pin_, LOW);
        
        SPI.transfer(addr);  // Read operation
        uint32_t data = 0;
        data |= SPI.transfer(0) << 24;
        data |= SPI.transfer(0) << 16;
        data |= SPI.transfer(0) << 8;
        data |= SPI.transfer(0);
        
        digitalWrite(cs_pin_, HIGH);
        SPI.endTransaction();
        return data;
    }
};
