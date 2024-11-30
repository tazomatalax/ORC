#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>

class WebInterface {
public:
    // Setpoint structure for all controllable parameters
    struct Setpoints {
        float temperature;    // °C
        float ph;            // pH units
        float dissolved_oxygen; // % saturation
        float stirring_speed;   // RPM
        float feed_rate;        // mL/min
        float pressure;         // PSI
    };

    // Control modes for different parameters
    struct ControlModes {
        enum class Mode {
            OFF,
            AUTO,
            MANUAL
        };

        Mode temperature;
        Mode ph;
        Mode dissolved_oxygen;
        Mode stirring;
        Mode feeding;
        Mode pressure;
    };

    // System status and current readings
    struct SystemStatus {
        float temperature[3];    // °C (multiple PT100 sensors)
        float ph;               // pH units
        float dissolved_oxygen; // % saturation
        float biomass;         // OD units
        float stirring_speed;   // RPM
        float feed_rate;        // mL/min
        float pressure;         // PSI
        bool heater_on;
        bool cooler_on;
        bool stirrer_on;
        bool pump_on;
        unsigned long uptime;   // milliseconds
    };

    void begin() {
        server.begin();
        setupRoutes();
        lastUpdate = 0;
        
        // Initialize default setpoints
        setpoints = {
            .temperature = 37.0f,
            .ph = 7.0f,
            .dissolved_oxygen = 40.0f,
            .stirring_speed = 200.0f,
            .feed_rate = 0.0f,
            .pressure = 1.0f
        };

        // Initialize all control modes to OFF
        control_modes = {};
    }

    void update() {
        server.handleClient();
        
        unsigned long currentTime = millis();
        if (currentTime - lastUpdate >= 1000) { // Update every second
            updateWebSocketClients();
            lastUpdate = currentTime;
        }
    }

private:
    WebServer server;
    unsigned long lastUpdate;
    Setpoints setpoints;
    ControlModes control_modes;
    SystemStatus status;

    void setupRoutes() {
        server.on("/", HTTP_GET, [this]() { handleRoot(); });
        server.on("/api/setpoints", HTTP_GET, [this]() { handleGetSetpoints(); });
        server.on("/api/setpoints", HTTP_POST, [this]() { handleSetpoints(); });
        server.on("/api/control", HTTP_GET, [this]() { handleGetControl(); });
        server.on("/api/control", HTTP_POST, [this]() { handleControl(); });
        server.on("/api/data", HTTP_GET, [this]() { handleData(); });
        server.on("/api/calibration", HTTP_POST, [this]() { handleCalibration(); });
        server.on("/api/system", HTTP_GET, [this]() { handleSystem(); });
        
        // Static files
        server.on("/css/styles.css", HTTP_GET, [this]() { handleStyles(); });
        server.on("/js/main.js", HTTP_GET, [this]() { handleJavascript(); });
    }

    void handleRoot() {
        server.send(200, "text/html", generateHTML());
    }

    void handleGetSetpoints() {
        StaticJsonDocument<512> doc;
        
        doc["temperature"] = setpoints.temperature;
        doc["ph"] = setpoints.ph;
        doc["dissolved_oxygen"] = setpoints.dissolved_oxygen;
        doc["stirring_speed"] = setpoints.stirring_speed;
        doc["feed_rate"] = setpoints.feed_rate;
        doc["pressure"] = setpoints.pressure;
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }

    void handleSetpoints() {
        if (server.hasArg("plain")) {
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, server.arg("plain"));
            
            if (!error) {
                // Update setpoints if provided
                if (doc.containsKey("temperature")) setpoints.temperature = doc["temperature"];
                if (doc.containsKey("ph")) setpoints.ph = doc["ph"];
                if (doc.containsKey("dissolved_oxygen")) setpoints.dissolved_oxygen = doc["dissolved_oxygen"];
                if (doc.containsKey("stirring_speed")) setpoints.stirring_speed = doc["stirring_speed"];
                if (doc.containsKey("feed_rate")) setpoints.feed_rate = doc["feed_rate"];
                if (doc.containsKey("pressure")) setpoints.pressure = doc["pressure"];
                
                server.send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            }
        }
    }

    void handleGetControl() {
        StaticJsonDocument<512> doc;
        
        doc["temperature"] = static_cast<int>(control_modes.temperature);
        doc["ph"] = static_cast<int>(control_modes.ph);
        doc["dissolved_oxygen"] = static_cast<int>(control_modes.dissolved_oxygen);
        doc["stirring"] = static_cast<int>(control_modes.stirring);
        doc["feeding"] = static_cast<int>(control_modes.feeding);
        doc["pressure"] = static_cast<int>(control_modes.pressure);
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }

    void handleControl() {
        if (server.hasArg("plain")) {
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, server.arg("plain"));
            
            if (!error) {
                // Update control modes if provided
                if (doc.containsKey("temperature")) 
                    control_modes.temperature = static_cast<ControlModes::Mode>(doc["temperature"].as<int>());
                if (doc.containsKey("ph")) 
                    control_modes.ph = static_cast<ControlModes::Mode>(doc["ph"].as<int>());
                if (doc.containsKey("dissolved_oxygen")) 
                    control_modes.dissolved_oxygen = static_cast<ControlModes::Mode>(doc["dissolved_oxygen"].as<int>());
                if (doc.containsKey("stirring")) 
                    control_modes.stirring = static_cast<ControlModes::Mode>(doc["stirring"].as<int>());
                if (doc.containsKey("feeding")) 
                    control_modes.feeding = static_cast<ControlModes::Mode>(doc["feeding"].as<int>());
                if (doc.containsKey("pressure")) 
                    control_modes.pressure = static_cast<ControlModes::Mode>(doc["pressure"].as<int>());
                
                server.send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            }
        }
    }

    void handleData() {
        StaticJsonDocument<1024> doc;
        
        // Current readings
        JsonObject readings = doc.createNestedObject("readings");
        readings["temperature_1"] = status.temperature[0];
        readings["temperature_2"] = status.temperature[1];
        readings["temperature_3"] = status.temperature[2];
        readings["ph"] = status.ph;
        readings["dissolved_oxygen"] = status.dissolved_oxygen;
        readings["biomass"] = status.biomass;
        readings["stirring_speed"] = status.stirring_speed;
        readings["feed_rate"] = status.feed_rate;
        readings["pressure"] = status.pressure;

        // System status
        JsonObject sys_status = doc.createNestedObject("status");
        sys_status["heater"] = status.heater_on;
        sys_status["cooler"] = status.cooler_on;
        sys_status["stirrer"] = status.stirrer_on;
        sys_status["pump"] = status.pump_on;
        sys_status["uptime"] = status.uptime;
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }

    void handleCalibration() {
        if (server.hasArg("plain")) {
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, server.arg("plain"));
            
            if (!error) {
                String sensor = doc["sensor"].as<String>();
                String action = doc["action"].as<String>();
                float value = doc["value"] | 0.0f;
                
                // Handle calibration based on sensor type
                bool success = performCalibration(sensor, action, value);
                
                if (success) {
                    server.send(200, "application/json", "{\"status\":\"success\"}");
                } else {
                    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Calibration failed\"}");
                }
            } else {
                server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            }
        }
    }

    void handleSystem() {
        StaticJsonDocument<256> doc;
        doc["version"] = "1.0.0";
        doc["uptime"] = millis();
        // Add other system information as needed
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }

    void updateWebSocketClients() {
        // Update status from actual sensors and controllers
        updateSystemStatus();
        // Implement WebSocket updates for real-time data
    }

    bool performCalibration(const String& sensor, const String& action, float value) {
        // Implement calibration logic for different sensors
        // Return true if calibration was successful
        return false;
    }

    void updateSystemStatus() {
        // Update status structure with current readings and system state
        status.uptime = millis();
    }

    String generateHTML() {
        // Return a comprehensive HTML interface
        return R"(
<!DOCTYPE html>
<html>
<head>
    <title>Bioreactor Control System</title>
    <link rel="stylesheet" href="/css/styles.css">
</head>
<body>
    <div id="app">
        <h1>Bioreactor Control System</h1>
        <div class="container">
            <!-- Add comprehensive UI elements here -->
        </div>
    </div>
    <script src="/js/main.js"></script>
</body>
</html>
)";
    }

    void handleStyles() {
        // Serve CSS file
        server.send(200, "text/css", "/* Add your CSS styles here */");
    }

    void handleJavascript() {
        // Serve JavaScript file
        server.send(200, "text/javascript", "// Add your JavaScript code here");
    }
};
