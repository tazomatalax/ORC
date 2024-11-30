#pragma once

#include <Arduino.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

class DatabaseManager {
public:
    void begin() {
        // InfluxDB connection parameters
        client.setConnectionParams(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
        
        // Check server connection
        if (client.validateConnection()) {
            Serial.println("Connected to InfluxDB");
        } else {
            Serial.println("InfluxDB connection failed");
        }

        // Configure data point
        sensor.addTag("device", "bioreactor");
        sensor.addTag("location", "lab");
    }

    void logSensorData(float ph, float do_level, float temp, float pressure) {
        // Clear fields from previous use
        sensor.clearFields();
        
        // Add fields
        sensor.addField("ph", ph);
        sensor.addField("dissolved_oxygen", do_level);
        sensor.addField("temperature", temp);
        sensor.addField("pressure", pressure);
        
        // Write point
        if (!client.writePoint(sensor)) {
            Serial.println("InfluxDB write failed");
        }
    }

    void logControlAction(const char* controller, const char* action, float value) {
        Point event("control_actions");
        event.addTag("controller", controller);
        event.addTag("action", action);
        event.addField("value", value);
        
        if (!client.writePoint(event)) {
            Serial.println("InfluxDB write failed");
        }
    }

private:
    InfluxDBClient client;
    Point sensor = Point("bioreactor_sensors");
    
    // InfluxDB connection details
    const char* INFLUXDB_URL = "http://localhost:8086";
    const char* INFLUXDB_TOKEN = "your-token";
    const char* INFLUXDB_ORG = "your-org";
    const char* INFLUXDB_BUCKET = "bioreactor";
};
