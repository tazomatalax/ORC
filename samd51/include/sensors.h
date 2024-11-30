#pragma once
#include <Arduino.h>
#include <Adafruit_MAX31865.h>

class SensorManager {
public:
    void begin();
    void update();
    float getPT100Temperature(uint8_t index);
    void readRS485Data();
    
private:
    static const uint8_t MAX_PT100_SENSORS = 4;
    static const uint8_t MAX_RS485_DEVICES = 8;
    
    Adafruit_MAX31865 pt100Sensors[MAX_PT100_SENSORS];
    float temperatures[MAX_PT100_SENSORS];
    
    void initPT100Sensors();
    void initRS485();
    void updatePT100Readings();
    void handleRS485Communication();
};
