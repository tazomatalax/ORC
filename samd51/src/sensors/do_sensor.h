#pragma once

#include "modbus_sensor.h"

class DOSensor : public ModbusSensor {
public:
    struct DOReading {
        float dissolvedOxygen;
        float temperature;
        bool valid;
    };

    DOSensor(HardwareSerial* serialPort, uint8_t addr = 3) 
        : ModbusSensor(serialPort, addr) {}

    DOReading read() {
        DOReading result = {0.0f, 0.0f, false};
        if (!initialized) return result;

        // Read DO and temperature registers
        uint16_t registers[10];
        if (modbus.readHoldingRegisters(slaveAddr, 2089, registers, 10)) {
            // Convert registers to float values
            result.dissolvedOxygen = registersToFloat(registers[2], registers[3]);
            result.temperature = registersToFloat(registers[6], registers[7]);
            result.valid = true;
        }

        return result;
    }
};
