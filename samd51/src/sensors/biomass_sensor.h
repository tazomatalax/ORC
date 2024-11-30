#pragma once

#include "modbus_sensor.h"

class BiomassSensor : public ModbusSensor {
public:
    struct BiomassReading {
        float density;
        float scattered_light;
        float transmitted_light;
        bool valid;
    };

    BiomassSensor(HardwareSerial* serialPort, uint8_t addr = 5) 
        : ModbusSensor(serialPort, addr) {}

    BiomassReading read() {
        BiomassReading result = {0.0f, 0.0f, 0.0f, false};
        if (!initialized) return result;

        // Read biomass measurement registers
        uint16_t registers[12];
        if (modbus.readHoldingRegisters(slaveAddr, 3000, registers, 12)) {
            // Convert registers to float values
            result.density = registersToFloat(registers[0], registers[1]);
            result.scattered_light = registersToFloat(registers[4], registers[5]);
            result.transmitted_light = registersToFloat(registers[8], registers[9]);
            result.valid = true;
        }

        return result;
    }
};
