#pragma once

#include <Arduino.h>
#include <PID_v1.h>
#include "../sensors/sensor_manager.h"

class TemperatureController {
public:
    static const int HEATER_PIN = 32;  // PB10
    static const int PWM_FREQUENCY = 1000;  // 1 kHz PWM frequency
    static const int PWM_RESOLUTION = 12;   // 12-bit resolution (0-4095)
    static const int PWM_MAX_DUTY = (1 << PWM_RESOLUTION) - 1;  // Maximum duty cycle value

    struct TemperatureReadings {
        float phTemp;
        float doTemp;
        bool phValid;
        bool doValid;
        float averageTemp;
    };

    TemperatureController(SensorManager& sensorManager) 
        : pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT),
          sensorManager(sensorManager) {
        lastControlAction = 0;
        lastMeasurement = 0;
        controlInterval = 10000; // Start with 10 second interval
        
        // Configure PID output range to match PWM resolution
        pid.SetOutputLimits(0, PWM_MAX_DUTY);
    }

    void begin() {
        // Configure heater PWM pin
        pinMode(HEATER_PIN, OUTPUT);
        
        // Configure PWM for TC4 on PB10
        pinPeripheral(HEATER_PIN, PIO_TIMER);
        
        // Set up Timer/Counter for PWM
        GCLK->PCHCTRL[TC4_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | GCLK_PCHCTRL_CHEN;
        while (GCLK->SYNCBUSY.reg);

        TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
        while (TC4->COUNT16.SYNCBUSY.bit.SWRST);
        
        TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                                TC_CTRLA_PRESCALER_DIV16 |
                                TC_CTRLA_PRESCSYNC_PRESC;
                                
        TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NPWM;
        
        TC4->COUNT16.CC[0].reg = PWM_MAX_DUTY;  // Period
        TC4->COUNT16.CC[1].reg = 0;             // Initial duty cycle
        
        while (TC4->COUNT16.SYNCBUSY.bit.CC0 || TC4->COUNT16.SYNCBUSY.bit.CC1);
        
        TC4->COUNT16.CTRLA.bit.ENABLE = 1;
        while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);

        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(controlInterval);
    }

    void setSetpoint(double newSetpoint) {
        setpoint = newSetpoint;
    }

    double getSetpoint() const {
        return setpoint;
    }

    TemperatureReadings getTemperatures() const {
        return lastReadings;
    }

    double getCurrentTemperature() const {
        return input;
    }

    double getHeaterOutput() const {
        return output;
    }

    void setControlInterval(unsigned long interval) {
        controlInterval = constrain(interval, 10000, 30000); // 10-30 seconds
        pid.SetSampleTime(controlInterval);
    }

    void setPIDTunings(double kp, double ki, double kd) {
        pid.SetTunings(kp, ki, kd);
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
    SensorManager& sensorManager;
    double input, output, setpoint;
    const double Kp = 2.0, Ki = 0.5, Kd = 0.1; // PID constants
    PID pid;
    unsigned long lastControlAction;
    unsigned long lastMeasurement;
    unsigned long controlInterval;
    TemperatureReadings lastReadings;

    double readTemperatureSensor() {
        // Get readings from sensor manager
        SensorManager::SensorReadings readings = sensorManager.read();

        // Update last readings structure
        lastReadings.phTemp = readings.ph_reading.temperature;
        lastReadings.doTemp = readings.do_reading.temperature;
        lastReadings.phValid = readings.ph_reading.valid;
        lastReadings.doValid = readings.do_reading.valid;

        // Calculate average temperature based on valid readings
        float sum = 0;
        int validCount = 0;

        if (readings.ph_reading.valid) {
            sum += readings.ph_reading.temperature;
            validCount++;
        }

        if (readings.do_reading.valid) {
            sum += readings.do_reading.temperature;
            validCount++;
        }

        // If no valid readings, return last known temperature
        if (validCount == 0) {
            return input;
        }

        lastReadings.averageTemp = sum / validCount;
        return lastReadings.averageTemp;
    }

    void adjustHeatingJacket(double pwmValue) {
        // Ensure PWM value is within bounds
        pwmValue = constrain(pwmValue, 0, PWM_MAX_DUTY);
        
        // Update PWM duty cycle
        TC4->COUNT16.CC[1].reg = static_cast<uint16_t>(pwmValue);
        while (TC4->COUNT16.SYNCBUSY.bit.CC1);
    }
};
