#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "Logger.h"

// Motor direction enum
enum MotorDirection {
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_STOPPED
};

class Motor {
public:
    // Constructor
    Motor(uint8_t forwardPin, uint8_t backwardPin, float calibration = 1.0);
    
    // Initialize the motor
    void begin();
    
    // Motor control functions
    void forward(uint8_t power);
    void backward(uint8_t power);
    void stop();
    
    // Get current state
    MotorDirection getDirection() const;
    uint8_t getPower() const;
    
    // Calibration
    void setCalibration(float calibration);
    float getCalibration() const;
    
private:
    // Motor pins
    uint8_t _forwardPin;
    uint8_t _backwardPin;
    
    // Current state
    MotorDirection _direction;
    uint8_t _power;
    
    // Calibration
    float _calibration;
    
    // Last reported state (for debugging)
    MotorDirection _lastReportedDirection;
    uint8_t _lastReportedPower;
    
    // Apply power to pins
    void applyPower(uint8_t forwardPower, uint8_t backwardPower);
};

#endif // MOTOR_H