#pragma once

#include <Arduino.h>
#include <PID_v1.h>

class PressureController {
public:
    PressureController() : pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT) {
        lastControlAction = 0;
        lastMeasurement = 0;
        controlInterval = 5000; // Start with 5 second interval
    }

    void begin() {
        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(controlInterval);
    }

    void setSetpoint(double newSetpoint) {
        setpoint = newSetpoint;
    }

    void setControlInterval(unsigned long interval) {
        controlInterval = constrain(interval, 5000, 10000); // 5-10 seconds
        pid.SetSampleTime(controlInterval);
    }

    void update() {
        unsigned long currentTime = millis();
        
        // Take measurement every second
        if (currentTime - lastMeasurement >= 1000) {
            input = readPressureSensor();
            lastMeasurement = currentTime;
        }

        // Control action based on control interval
        if (currentTime - lastControlAction >= controlInterval) {
            pid.Compute();
            adjustBackpressure(output);
            lastControlAction = currentTime;
        }
    }

private:
    double input, output, setpoint;
    const double Kp = 1.0, Ki = 0.2, Kd = 0.05; // PID constants
    PID pid;
    unsigned long lastControlAction;
    unsigned long lastMeasurement;
    unsigned long controlInterval;

    double readPressureSensor() {
        // TODO: Implement actual pressure sensor reading
        return 0.0;
    }

    void adjustBackpressure(double value) {
        // TODO: Implement backpressure control
    }
};
