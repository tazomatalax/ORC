#pragma once

#include <Arduino.h>
#include <SPI.h>

class PT100Sensor {
public:
    struct PT100Reading {
        float temperature;
        bool valid;
        bool fault;
        uint8_t fault_code;
    };

    struct PT100Readings {
        PT100Reading sensors[3];
        unsigned long timestamp;
        bool valid;
    };

    PT100Sensor(uint8_t cs1, uint8_t cs2, uint8_t cs3, uint8_t drdy1, uint8_t drdy2, uint8_t drdy3)
        : cs_pins{cs1, cs2, cs3}
        , drdy_pins{drdy1, drdy2, drdy3}
        , initialized(false) {}

    bool begin() {
        SPI.begin();
        
        for(int i = 0; i < 3; i++) {
            pinMode(cs_pins[i], OUTPUT);
            digitalWrite(cs_pins[i], HIGH);
            pinMode(drdy_pins[i], INPUT);
        }

        // Configure each MAX31865
        for(int i = 0; i < 3; i++) {
            // Set to 4-wire mode, automatic bias on, 50Hz filter
            writeRegister(i, 0x00, 0xC3);
            
            // Set RTD high and low threshold - optional
            writeRegister(i, 0x03, 0xFF); // High threshold
            writeRegister(i, 0x04, 0x00); // Low threshold
        }

        initialized = true;
        return true;
    }

    PT100Readings read() {
        PT100Readings readings;
        readings.timestamp = millis();
        readings.valid = false;

        if (!initialized) {
            return readings;
        }

        bool any_valid = false;
        for(int i = 0; i < 3; i++) {
            readings.sensors[i] = readSensor(i);
            if (readings.sensors[i].valid) {
                any_valid = true;
            }
        }

        readings.valid = any_valid;
        return readings;
    }

private:
    uint8_t cs_pins[3];
    uint8_t drdy_pins[3];
    bool initialized;

    void writeRegister(uint8_t sensor_idx, uint8_t reg, uint8_t value) {
        digitalWrite(cs_pins[sensor_idx], LOW);
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
        SPI.transfer(reg | 0x80); // Set write bit
        SPI.transfer(value);
        SPI.endTransaction();
        digitalWrite(cs_pins[sensor_idx], HIGH);
    }

    uint8_t readRegister(uint8_t sensor_idx, uint8_t reg) {
        digitalWrite(cs_pins[sensor_idx], LOW);
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
        SPI.transfer(reg & 0x7F); // Clear write bit
        uint8_t value = SPI.transfer(0xFF);
        SPI.endTransaction();
        digitalWrite(cs_pins[sensor_idx], HIGH);
        return value;
    }

    PT100Reading readSensor(uint8_t sensor_idx) {
        PT100Reading reading;
        reading.valid = false;
        reading.fault = false;

        // Read temperature registers
        digitalWrite(cs_pins[sensor_idx], LOW);
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
        SPI.transfer(0x01 & 0x7F); // Start at register 1
        uint16_t rtd = (SPI.transfer(0xFF) << 8);
        rtd |= SPI.transfer(0xFF);
        SPI.endTransaction();
        digitalWrite(cs_pins[sensor_idx], HIGH);

        // Check fault status
        uint8_t fault_status = readRegister(sensor_idx, 0x07);
        if (fault_status) {
            reading.fault = true;
            reading.fault_code = fault_status;
            return reading;
        }

        // Remove fault bit
        rtd >>= 1;

        // Convert RTD value to temperature
        // For PT100: R(t) = R0(1 + At + Bt^2)
        // where R0 = 100Ω, A = 3.9083e-3, B = -5.775e-7
        float resistance = rtd * 400.0 / 32768.0; // Reference resistor is 400Ω
        float temp = ((resistance / 100.0) - 1.0) / 0.00385; // Simplified calculation

        reading.temperature = temp;
        reading.valid = true;
        return reading;
    }
};
