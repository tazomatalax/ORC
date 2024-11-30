#pragma once

#include <Arduino.h>
#include <PID_v1.h>

class TemperatureController {
public:
    TemperatureController() : pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT) {
        lastControlAction = 0;
        lastMeasurement = 0;
        controlInterval = 10000; // Start with 10 second interval
    }

    void begin() {
        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(controlInterval);
    }

    void setSetpoint(double newSetpoint) {
        setpoint = newSetpoint;
    }

    void setControlInterval(unsigned long interval) {
        controlInterval = constrain(interval, 10000, 30000); // 10-30 seconds
        pid.SetSampleTime(controlInterval);
    }

    void update() {
        unsigned long currentTime = millis();
        
        // Take measurement every second
        if (currentTime - lastMeasurement >= 1000) {
            input = readTemperatureSensor();
            lastMeasurement = currentTime;
        }

        // Control action based on control interval
        if (currentTime - lastControlAction >= controlInterval) {
            pid.Compute();
            adjustHeatingJacket(output);
            lastControlAction = currentTime;
        }
    }

private:
    double input, output, setpoint;
    const double Kp = 2.0, Ki = 0.5, Kd = 0.1; // PID constants
    PID pid;
    unsigned long lastControlAction;
    unsigned long lastMeasurement;
    unsigned long controlInterval;

    double readTemperatureSensor() {
        // TODO: Implement actual temperature sensor reading
        return 0.0;
    }

    void adjustHeatingJacket(double value) {
        // TODO: Implement heating jacket control
    }
};
