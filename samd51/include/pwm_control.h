#pragma once
#include <Arduino.h>

class PWMController {
public:
    void begin();
    void update();
    void setPWM(uint8_t channel, uint16_t value);
    uint16_t getPWM(uint8_t channel);
    
private:
    static const uint8_t NUM_PWM_CHANNELS = 8;
    uint16_t pwmValues[NUM_PWM_CHANNELS];
    
    void initPWMChannels();
    void updatePWMOutputs();
};
