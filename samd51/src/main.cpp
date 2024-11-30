#include <Arduino.h>
#include "sensors/sensor_manager.h"
#include "stepper.h"
#include "communication.h"
#include "pwm_control.h"
#include "controllers/controller_manager.h"

// Global objects
SensorManager sensors;
StepperController steppers;
CommunicationManager comm;
PWMController pwm;
ControllerManager controllers(sensors);  // Pass sensors to controller manager

// Function to update controllers with sensor readings
void updateControllersWithSensorData(const SensorManager::SensorReadings& readings) {
    if (readings.ph_reading.valid) {
        controllers.getPHController().setCurrentValue(readings.ph_reading.pH);
    }
    
    if (readings.do_reading.valid) {
        controllers.getDOController().setCurrentValue(readings.do_reading.dissolvedOxygen);
    }
    
    // Temperature is now handled directly by the TemperatureController
    // No need to manually set it here
}

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    // Initialize subsystems
    if (!sensors.begin()) {
        Serial.println("Failed to initialize sensors!");
    }
    
    steppers.begin();
    comm.begin();
    pwm.begin();
    controllers.begin();

    // Set initial setpoints
    controllers.getPHController().setSetpoint(7.0);
    controllers.getDOController().setSetpoint(40.0);
    controllers.getTemperatureController().setSetpoint(37.0);
    controllers.getPressureController().setSetpoint(1.0);
}

void loop() {
    // Update all subsystems
    sensors.update();
    
    // Get the latest sensor readings
    const auto& readings = sensors.getLastValidReadings();
    
    // Update controllers with sensor data
    updateControllersWithSensorData(readings);
    
    // Update control systems
    steppers.update();
    controllers.update();
    
    // Handle communication with RP2040
    comm.handleCommunication();
    
    // Update PWM outputs
    pwm.update();
    
    // Small delay to prevent tight looping
    delay(1);
}
