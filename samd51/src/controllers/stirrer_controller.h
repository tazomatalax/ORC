#pragma once

#include "stepper_controller.h"

class StirrerController {
public:
    StirrerController(uint8_t cs_pin, uint8_t en_pin) 
        : stepper_(cs_pin, en_pin), current_rpm_(0), target_rpm_(0) {}

    void begin() {
        stepper_.begin();
    }

    void enable() {
        stepper_.enable();
    }

    void disable() {
        stepper_.disable();
    }

    // Set stirring speed in RPM
    void setSpeed(float rpm) {
        if (rpm < 0) rpm = 0;
        if (rpm > MAX_RPM) rpm = MAX_RPM;
        
        target_rpm_ = rpm;
        // Convert RPM to internal velocity units
        // Assuming 200 steps per revolution and 256 microsteps
        uint32_t steps_per_second = (rpm * 200 * 256) / 60;
        stepper_.setSpeed(steps_per_second);
    }

    float getCurrentSpeed() {
        int32_t current_velocity = stepper_.getCurrentVelocity();
        // Convert internal velocity units back to RPM
        current_rpm_ = (current_velocity * 60.0f) / (200 * 256);
        return current_rpm_;
    }

    void stop() {
        target_rpm_ = 0;
        stepper_.stop();
    }

private:
    StepperController stepper_;
    float current_rpm_;
    float target_rpm_;
    static constexpr float MAX_RPM = 3000.0f;  // Maximum RPM for the stirrer
};
