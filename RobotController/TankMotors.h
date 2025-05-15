#ifndef TANK_MOTORS_H
#define TANK_MOTORS_H

#include <Arduino.h>

// Motor direction enum
enum MotorDirection
{
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_STOPPED
};

// Default settings
#define DEFAULT_LEFT_CALIBRATION 1.0
#define DEFAULT_RIGHT_CALIBRATION 1.0
#define DEFAULT_MOTOR_DEBUG_ENABLED false

class TankMotors
{
public:
    // Constructor
    TankMotors(uint8_t leftForwardPin, uint8_t leftBackwardPin,
               uint8_t rightForwardPin, uint8_t rightBackwardPin);

    // Initialize motors
    void begin();

    // Basic motor control
    void leftForward(uint8_t power);
    void leftBackward(uint8_t power);
    void rightForward(uint8_t power);
    void rightBackward(uint8_t power);
    void leftStop();
    void rightStop();
    void stop();

    // Calibration
    void setLeftCalibration(float calibration);
    void setRightCalibration(float calibration);
    float getLeftCalibration() const;
    float getRightCalibration() const;

    // Get motor state
    MotorDirection getLeftDirection() const;
    MotorDirection getRightDirection() const;
    uint8_t getLeftPower() const;
    uint8_t getRightPower() const;

private:
    // Motor pins
    uint8_t _leftForwardPin;
    uint8_t _leftBackwardPin;
    uint8_t _rightForwardPin;
    uint8_t _rightBackwardPin;

    // Current state
    MotorDirection _leftDirection;
    MotorDirection _rightDirection;
    uint8_t _leftPower;
    uint8_t _rightPower;

    // Calibration
    float _leftCalibration;
    float _rightCalibration;

    // Helper methods
    void applyLeftPower(uint8_t forwardPower, uint8_t backwardPower);
    void applyRightPower(uint8_t forwardPower, uint8_t backwardPower);
};

#endif // TANK_MOTORS_H