#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTHandler {
public:
    void begin(Client& networkClient) {
        mqtt.setClient(networkClient);
        mqtt.setServer(MQTT_SERVER, MQTT_PORT);
        lastPublish = 0;
    }

    void update() {
        if (!mqtt.connected()) {
            reconnect();
        }
        mqtt.loop();

        unsigned long currentTime = millis();
        if (currentTime - lastPublish >= 1000) { // Publish every second
            publishSensorData();
            lastPublish = currentTime;
        }
    }

private:
    PubSubClient mqtt;
    unsigned long lastPublish;
    const char* MQTT_SERVER = "localhost";
    const int MQTT_PORT = 1883;

    void reconnect() {
        while (!mqtt.connected()) {
            if (mqtt.connect("BioreactorController")) {
                // Subscribe to control topics
                mqtt.subscribe("bioreactor/control/ph/setpoint");
                mqtt.subscribe("bioreactor/control/do/setpoint");
                mqtt.subscribe("bioreactor/control/temperature/setpoint");
                mqtt.subscribe("bioreactor/control/pressure/setpoint");
            } else {
                delay(5000);
            }
        }
    }

    void publishSensorData() {
        StaticJsonDocument<200> doc;
        
        // Create JSON structure
        doc["timestamp"] = millis();
        doc["ph"] = 7.0;  // TODO: Get actual values
        doc["do"] = 40.0;
        doc["temperature"] = 37.0;
        doc["pressure"] = 1.0;

        char buffer[200];
        serializeJson(doc, buffer);
        
        // Publish to different topics
        mqtt.publish("bioreactor/data/ph", buffer);
        mqtt.publish("bioreactor/data/do", buffer);
        mqtt.publish("bioreactor/data/temperature", buffer);
        mqtt.publish("bioreactor/data/pressure", buffer);
    }
};
