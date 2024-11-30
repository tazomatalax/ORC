#pragma once
#include <Arduino.h>
#include <TMCStepper.h>

class StepperController {
public:
    void begin();
    void update();
    void setSpeed(uint8_t motor, float speed);
    void setPosition(uint8_t motor, long position);
    void enableMotor(uint8_t motor, bool enable);
    
private:
    static const uint8_t NUM_MOTORS = 4;
    TMCStepper* drivers[NUM_MOTORS];
    
    struct MotorState {
        bool enabled;
        float currentSpeed;
        long targetPosition;
        long currentPosition;
    } motorStates[NUM_MOTORS];
    
    void initDrivers();
    void updateMotorPositions();
};
