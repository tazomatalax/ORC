#pragma once

#include <Arduino.h>
#include <PID_v1.h>

class PHController {
public:
    PHController() : pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT) {
        lastControlAction = 0;
        lastMeasurement = 0;
    }

    void begin() {
        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(2000); // 2 seconds
    }

    void setSetpoint(double newSetpoint) {
        setpoint = newSetpoint;
    }

    void update() {
        unsigned long currentTime = millis();
        
        // Take measurement every second
        if (currentTime - lastMeasurement >= 1000) {
            input = readPHSensor();
            lastMeasurement = currentTime;
        }

        // Control action every 2 minutes
        if (currentTime - lastControlAction >= 120000) {
            if (setpoint - input >= 0.2) { // Trigger condition: 0.2 pH below setpoint
                pid.Compute();
                actuatePump(output);
                logToDatabase();
            }
            lastControlAction = currentTime;
        }
    }

private:
    double input, output, setpoint;
    const double Kp = 2.0, Ki = 0.5, Kd = 0.1; // PID constants
    PID pid;
    unsigned long lastControlAction;
    unsigned long lastMeasurement;

    double readPHSensor() {
        // TODO: Implement actual pH sensor reading
        return 0.0;
    }

    void actuatePump(double value) {
        // TODO: Implement pump control
    }

    void logToDatabase() {
        // TODO: Implement database logging
    }
};
