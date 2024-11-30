#pragma once

#include "do_sensor.h"
#include "ph_sensor.h"
#include "biomass_sensor.h"
#include "pt100_sensor.h"

class SensorManager {
public:
    struct SensorReadings {
        DOSensor::DOReading do_reading;
        PHSensor::PHReading ph_reading;
        BiomassSensor::BiomassReading biomass_reading;
        PT100Sensor::PT100Readings pt100_reading;
        unsigned long timestamp;
    };

    SensorManager()
        : doSensor(&Serial1, 3)
        , phSensor(&Serial2, 4)
        , biomassSensor(&Serial3, 5)
        , pt100Sensor(PT100_CS_1, PT100_CS_2, PT100_CS_3,
                     PT100_IRQ_1, PT100_IRQ_2, PT100_IRQ_3)
        , lastReadTime(0)
    {}

    bool begin() {
        bool success = true;
        success &= doSensor.begin();
        success &= phSensor.begin();
        success &= biomassSensor.begin();
        success &= pt100Sensor.begin();
        return success;
    }

    SensorReadings read() {
        SensorReadings readings;
        readings.timestamp = millis();
        
        // Read all sensors
        readings.do_reading = doSensor.read();
        readings.ph_reading = phSensor.read();
        readings.biomass_reading = biomassSensor.read();
        readings.pt100_reading = pt100Sensor.read();
        
        lastReadTime = readings.timestamp;
        return readings;
    }

    // Update function to be called in the main loop
    void update() {
        unsigned long currentTime = millis();
        
        // Read sensors every second
        if (currentTime - lastReadTime >= 1000) {
            SensorReadings readings = read();
            
            // Store the readings in the circular buffer
            readings_buffer[buffer_index] = readings;
            buffer_index = (buffer_index + 1) % BUFFER_SIZE;
            
            // Update the last valid readings if the new readings are valid
            if (readings.do_reading.valid) last_valid_readings.do_reading = readings.do_reading;
            if (readings.ph_reading.valid) last_valid_readings.ph_reading = readings.ph_reading;
            if (readings.biomass_reading.valid) last_valid_readings.biomass_reading = readings.biomass_reading;
        }
    }

    // Get the most recent valid readings
    const SensorReadings& getLastValidReadings() const {
        return last_valid_readings;
    }

private:
    static const uint8_t PT100_CS_1 = 13;  // PT100_CS_1 from schematic
    static const uint8_t PT100_CS_2 = 13;  // PT100_CS_2 from schematic
    static const uint8_t PT100_CS_3 = 13;  // PT100_CS_3 from schematic
    static const uint8_t PT100_IRQ_1 = 18; // PT100_IRQ_1 from schematic
    static const uint8_t PT100_IRQ_2 = 18; // PT100_IRQ_2 from schematic
    static const uint8_t PT100_IRQ_3 = 18; // PT100_IRQ_3 from schematic

    DOSensor doSensor;
    PHSensor phSensor;
    BiomassSensor biomassSensor;
    PT100Sensor pt100Sensor;
    unsigned long lastReadTime;

    static const size_t BUFFER_SIZE = 60; // Store 1 minute of readings
    SensorReadings readings_buffer[BUFFER_SIZE];
    size_t buffer_index = 0;
    SensorReadings last_valid_readings;
};
