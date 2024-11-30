#pragma once

#include <Arduino.h>
#include <ModbusRTU.h>

class ModbusSensor {
protected:
    ModbusRTU modbus;
    uint8_t slaveAddr;
    HardwareSerial* serial;
    bool initialized;

    // Helper function to convert two 16-bit registers to float
    float registersToFloat(uint16_t reg1, uint16_t reg2) {
        uint32_t combined = ((uint32_t)reg2 << 16) | reg1;
        float result;
        memcpy(&result, &combined, 4);
        return result;
    }

public:
    ModbusSensor(HardwareSerial* serialPort, uint8_t addr) 
        : serial(serialPort), slaveAddr(addr), initialized(false) {}

    bool begin() {
        serial->begin(19200, SERIAL_8N2);
        modbus.begin(serial, 1); // DE/RE pin for RS485
        modbus.master();
        initialized = true;
        return true;
    }

    bool isInitialized() const {
        return initialized;
    }
};
