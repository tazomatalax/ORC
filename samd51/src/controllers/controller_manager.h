#pragma once

#include "ph_controller.h"
#include "do_controller.h"
#include "temperature_controller.h"
#include "pressure_controller.h"
#include "../safety/safety_manager.h"

class ControllerManager {
public:
    void begin() {
        phController.begin();
        doController.begin();
        tempController.begin();
        pressureController.begin();
        safetyManager.begin();
    }

    void update() {
        // Only update controllers if safety checks pass
        if (safetyManager.isSystemSafe()) {
            phController.update();
            doController.update();
            tempController.update();
            pressureController.update();
        } else {
            safetyManager.handleUnsafeCondition();
        }
    }

    // Getters for individual controllers
    PHController& getPHController() { return phController; }
    DOController& getDOController() { return doController; }
    TemperatureController& getTemperatureController() { return tempController; }
    PressureController& getPressureController() { return pressureController; }

private:
    PHController phController;
    DOController doController;
    TemperatureController tempController;
    PressureController pressureController;
    SafetyManager safetyManager;
};
