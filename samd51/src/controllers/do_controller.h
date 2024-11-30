#pragma once

#include <Arduino.h>
#include <PID_v1.h>

class DOController {
public:
    DOController() : stirrerPID(&input, &stirrerOutput, &setpoint, Kp_s, Ki_s, Kd_s, DIRECT),
                    gasPID(&input, &gasOutput, &setpoint, Kp_g, Ki_g, Kd_g, DIRECT) {
        lastControlAction = 0;
        lastMeasurement = 0;
        cascadePriority = CascadePriority::STIRRER_FIRST;
    }

    enum class CascadePriority {
        STIRRER_FIRST,
        GAS_FIRST
    };

    void begin() {
        stirrerPID.SetMode(AUTOMATIC);
        gasPID.SetMode(AUTOMATIC);
        stirrerPID.SetSampleTime(30000); // 30 seconds
        gasPID.SetSampleTime(30000);     // 30 seconds
    }

    void setCascadePriority(CascadePriority priority) {
        cascadePriority = priority;
    }

    void setSetpoint(double newSetpoint) {
        setpoint = newSetpoint;
    }

    void update() {
        unsigned long currentTime = millis();
        
        // Take measurement every second
        if (currentTime - lastMeasurement >= 1000) {
            input = readDOSensor();
            lastMeasurement = currentTime;
        }

        // Control action every 30 seconds
        if (currentTime - lastControlAction >= 30000) {
            if (cascadePriority == CascadePriority::STIRRER_FIRST) {
                updateStirrerFirst();
            } else {
                updateGasFirst();
            }
            lastControlAction = currentTime;
        }
    }

private:
    double input, stirrerOutput, gasOutput, setpoint;
    const double Kp_s = 2.0, Ki_s = 0.5, Kd_s = 0.1; // Stirrer PID constants
    const double Kp_g = 1.0, Ki_g = 0.2, Kd_g = 0.05; // Gas PID constants
    PID stirrerPID;
    PID gasPID;
    CascadePriority cascadePriority;
    unsigned long lastControlAction;
    unsigned long lastMeasurement;

    void updateStirrerFirst() {
        stirrerPID.Compute();
        if (stirrerOutput >= 95) { // If stirrer near max capacity
            gasPID.Compute();
            adjustGasFlow(gasOutput);
        }
        adjustStirrerSpeed(stirrerOutput);
    }

    void updateGasFirst() {
        gasPID.Compute();
        if (gasOutput >= 95) { // If gas flow near max capacity
            stirrerPID.Compute();
            adjustStirrerSpeed(stirrerOutput);
        }
        adjustGasFlow(gasOutput);
    }

    double readDOSensor() {
        // TODO: Implement actual DO sensor reading
        return 0.0;
    }

    void adjustStirrerSpeed(double value) {
        // TODO: Implement stirrer speed control
    }

    void adjustGasFlow(double value) {
        // TODO: Implement gas flow control
    }
};
