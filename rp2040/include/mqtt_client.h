#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTClient {
public:
    void begin();
    void update();
    void publish(const char* topic, const JsonDocument& data);
    void subscribe(const char* topic);
    
private:
    PubSubClient client;
    bool connected;
    
    void connect();
    void handleCallback(char* topic, byte* payload, unsigned int length);
    static void staticCallback(char* topic, byte* payload, unsigned int length);
};
