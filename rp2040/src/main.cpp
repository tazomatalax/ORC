#include <Arduino.h>
#include "network_manager.h"
#include "web_server.h"
#include "data_logger.h"
#include "mqtt_client.h"
#include "samd_interface.h"
#include "data/mqtt_handler.h"
#include "data/database_manager.h"
#include "web/web_interface.h"

// Global objects
NetworkManager network;
MQTTHandler mqtt;
DatabaseManager db;
WebInterface webInterface;
SAMDInterface samd;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    // Initialize network first
    network.begin();
    
    // Initialize other subsystems
    mqtt.begin(network.getClient());
    db.begin();
    webInterface.begin();
    samd.begin();
}

void loop() {
    // Update all subsystems
    network.update();
    mqtt.update();
    webInterface.update();
    
    // Handle communication with SAMD51
    samd.update();
    
    // Get sensor data from SAMD51 and log it
    if (samd.hasNewData()) {
        float ph = samd.getPH();
        float do_level = samd.getDO();
        float temp = samd.getTemperature();
        float pressure = samd.getPressure();
        
        // Log to database
        db.logSensorData(ph, do_level, temp, pressure);
    }
    
    // Small delay to prevent tight looping
    delay(1);
}
