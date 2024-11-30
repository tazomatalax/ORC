#pragma once

#include "ph_controller.h"
#include "do_controller.h"
#include "temperature_controller.h"
#include "pressure_controller.h"
#include "stirrer_controller.h"
#include "stepper_controller.h"
#include "../safety/safety_manager.h"
#include "../sensors/sensor_manager.h"

// Pin definitions for various controllers
namespace ControllerPins {
    // Stirrer control pins
    constexpr uint8_t STIRRER_CS_PIN = 10;    // Chip select for TMC5130
    constexpr uint8_t STIRRER_EN_PIN = 11;    // Enable pin for TMC5130
    
    // Additional stepper motor pins
    constexpr uint8_t PUMP_CS_PIN = 12;       // Chip select for pump stepper
    constexpr uint8_t PUMP_EN_PIN = 13;       // Enable pin for pump stepper
    
    // PWM control pins
    constexpr uint8_t HEATER_PWM_PIN = 32;    // PB10 for heater control
    constexpr uint8_t PUMP_PWM_PIN = 33;      // For pump control
}

class ControllerManager {
public:
    ControllerManager(SensorManager& sensors)
        : tempController(sensors)
        , safetyManager(sensors)
        , stirrerController(ControllerPins::STIRRER_CS_PIN, ControllerPins::STIRRER_EN_PIN)
        , pumpStepper(ControllerPins::PUMP_CS_PIN, ControllerPins::PUMP_EN_PIN)
    {
        // Initialize default setpoints
        setpoints = {
            .ph = 7.0f,
            .dissolvedOxygen = 40.0f,
            .temperature = 37.0f,
            .pressure = 1.0f,
            .stirrerSpeed = 200.0f,
            .pumpSpeed = 0
        };
    }

    void begin() {
        // Initialize all controllers
        phController.begin();
        doController.begin();
        tempController.begin();
        pressureController.begin();
        stirrerController.begin();
        pumpStepper.begin();
        safetyManager.begin();

        // Set initial setpoints
        applySetpoints();
    }

    void update() {
        // Only update controllers if safety checks pass
        if (safetyManager.isSystemSafe()) {
            phController.update();
            doController.update();
            tempController.update();
            pressureController.update();
            
            // Update stirrer speed based on DO control requirements
            float requiredStirrerSpeed = doController.getRequiredStirrerSpeed();
            if (requiredStirrerSpeed > 0) {
                stirrerController.setSpeed(requiredStirrerSpeed);
            }

            // Update stepper positions and velocities
            pumpStepper.updatePosition();
        } else {
            handleSafetyShutdown();
        }
    }

    // Setpoint structure for all controllable parameters
    struct Setpoints {
        float ph;
        float dissolvedOxygen;
        float temperature;
        float pressure;
        float stirrerSpeed;
        int32_t pumpSpeed;
    };

    // Getters for individual controllers
    PHController& getPHController() { return phController; }
    DOController& getDOController() { return doController; }
    TemperatureController& getTemperatureController() { return tempController; }
    PressureController& getPressureController() { return pressureController; }
    StirrerController& getStirrerController() { return stirrerController; }
    StepperController& getPumpStepper() { return pumpStepper; }
    SafetyManager& getSafetyManager() { return safetyManager; }

    // Setpoint management
    void setSetpoints(const Setpoints& newSetpoints) {
        setpoints = newSetpoints;
        applySetpoints();
    }

    const Setpoints& getSetpoints() const {
        return setpoints;
    }

    // Stepper motor direct control methods
    void setPumpSpeed(int32_t speed) {
        setpoints.pumpSpeed = speed;
        pumpStepper.setSpeed(speed);
    }

    void stopPump() {
        setpoints.pumpSpeed = 0;
        pumpStepper.stop();
    }

    void enablePump() {
        pumpStepper.enable();
    }

    void disablePump() {
        pumpStepper.disable();
    }

    // Emergency stop
    void emergencyStop() {
        stirrerController.stop();
        pumpStepper.stop();
        pumpStepper.disable();
        tempController.setSetpoint(20.0); // Room temperature
        safetyManager.triggerEmergencyStop();
    }

private:
    // Controllers
    PHController phController;
    DOController doController;
    TemperatureController tempController;
    PressureController pressureController;
    StirrerController stirrerController;
    StepperController pumpStepper;
    SafetyManager safetyManager;

    // Current setpoints
    Setpoints setpoints;

    void applySetpoints() {
        phController.setSetpoint(setpoints.ph);
        doController.setSetpoint(setpoints.dissolvedOxygen);
        tempController.setSetpoint(setpoints.temperature);
        pressureController.setSetpoint(setpoints.pressure);
        stirrerController.setSpeed(setpoints.stirrerSpeed);
        pumpStepper.setSpeed(setpoints.pumpSpeed);
    }

    void handleSafetyShutdown() {
        // Stop all active controls
        stirrerController.stop();
        pumpStepper.stop();
        pumpStepper.disable();
        
        // Set safe states
        tempController.setSetpoint(20.0); // Room temperature
        
        // Disable PWM outputs
        analogWrite(ControllerPins::HEATER_PWM_PIN, 0);
        analogWrite(ControllerPins::PUMP_PWM_PIN, 0);
        
        // Log the safety shutdown
        // TODO: Implement logging
    }
};
