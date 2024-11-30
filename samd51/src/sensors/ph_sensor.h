#pragma once

#include "modbus_sensor.h"

class PHSensor : public ModbusSensor {
public:
    struct PHReading {
        float pH;
        float temperature;
        bool valid;
    };

    PHSensor(HardwareSerial* serialPort, uint8_t addr = 4) 
        : ModbusSensor(serialPort, addr) {}

    PHReading read() {
        PHReading result = {0.0f, 0.0f, false};
        if (!initialized) return result;

        // Read pH and temperature registers
        uint16_t registers[10];
        if (modbus.readHoldingRegisters(slaveAddr, 2409, registers, 10)) {
            // Convert registers to float values
            result.pH = registersToFloat(registers[2], registers[3]);
            result.temperature = registersToFloat(registers[6], registers[7]);
            result.valid = true;
        }

        return result;
    }
};
