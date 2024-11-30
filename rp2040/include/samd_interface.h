#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <ArduinoJson.h>

class SAMDInterface {
public:
    void begin();
    void update();
    void sendCommand(const JsonDocument& command);
    bool hasNewData();
    JsonDocument getLatestData();
    
private:
    static const uint8_t BUFFER_SIZE = 256;
    uint8_t txBuffer[BUFFER_SIZE];
    uint8_t rxBuffer[BUFFER_SIZE];
    bool newDataAvailable;
    
    void initSPI();
    void handleSPICommunication();
    void processReceivedData();
};
