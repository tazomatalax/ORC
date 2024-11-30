#pragma once
#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>

class DataLogger {
public:
    void begin();
    void update();
    void logData(const JsonDocument& data);
    bool isReady();
    
private:
    File currentLogFile;
    bool sdReady;
    
    void initSD();
    String getLogFileName();
    void rotateLogFile();
    void checkSDSpace();
};
