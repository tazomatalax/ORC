#pragma once
#include <Arduino.h>
#include <SPI.h>

class CommunicationManager {
public:
    void begin();
    void handleCommunication();
    void sendSensorData();
    void receiveCommands();
    
private:
    static const uint8_t BUFFER_SIZE = 256;
    uint8_t txBuffer[BUFFER_SIZE];
    uint8_t rxBuffer[BUFFER_SIZE];
    
    void initSPI();
    void processSPIData();
    void packSensorData();
};
