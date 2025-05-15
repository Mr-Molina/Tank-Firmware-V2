#include "TankMotors.h"

TankMotors::TankMotors(uint8_t leftForwardPin, uint8_t leftBackwardPin,
                       uint8_t rightForwardPin, uint8_t rightBackwardPin)
{
    // Store pin assignments
    _leftForwardPin = leftForwardPin;
    _leftBackwardPin = leftBackwardPin;
    _rightForwardPin = rightForwardPin;
    _rightBackwardPin = rightBackwardPin;

    // Initialize state
    _leftDirection = MOTOR_STOPPED;
    _rightDirection = MOTOR_STOPPED;
    _leftPower = 0;
    _rightPower = 0;

    // Set default calibration
    _leftCalibration = DEFAULT_LEFT_CALIBRATION;
    _rightCalibration = DEFAULT_RIGHT_CALIBRATION;
}

void TankMotors::begin()
{
    // Configure pins for output
    pinMode(_leftForwardPin, OUTPUT);
    pinMode(_leftBackwardPin, OUTPUT);
    pinMode(_rightForwardPin, OUTPUT);
    pinMode(_rightBackwardPin, OUTPUT);

    // Stop all motors
    stop();

    Serial.println("TankMotors initialized");
}

void TankMotors::leftForward(uint8_t power)
{
    _leftDirection = MOTOR_FORWARD;
    _leftPower = power;

    uint8_t calibratedPower = power * _leftCalibration;
    applyLeftPower(calibratedPower, 0);
}

void TankMotors::leftBackward(uint8_t power)
{
    _leftDirection = MOTOR_BACKWARD;
    _leftPower = power;

    uint8_t calibratedPower = power * _leftCalibration;
    applyLeftPower(0, calibratedPower);
}

void TankMotors::rightForward(uint8_t power)
{
    _rightDirection = MOTOR_FORWARD;
    _rightPower = power;

    uint8_t calibratedPower = power * _rightCalibration;
    applyRightPower(calibratedPower, 0);
}

void TankMotors::rightBackward(uint8_t power)
{
    _rightDirection = MOTOR_BACKWARD;
    _rightPower = power;

    uint8_t calibratedPower = power * _rightCalibration;
    applyRightPower(0, calibratedPower);
}

void TankMotors::leftStop()
{
    _leftDirection = MOTOR_STOPPED;
    _leftPower = 0;

    applyLeftPower(0, 0);
}

void TankMotors::rightStop()
{
    _rightDirection = MOTOR_STOPPED;
    _rightPower = 0;

    applyRightPower(0, 0);
}

void TankMotors::stop()
{
    leftStop();
    rightStop();
}

void TankMotors::setLeftCalibration(float calibration)
{
    _leftCalibration = constrain(calibration, 0.0, 1.0);
    Serial.printf("Left motor calibration: %.2f\n", _leftCalibration);
}

void TankMotors::setRightCalibration(float calibration)
{
    _rightCalibration = constrain(calibration, 0.0, 1.0);
    Serial.printf("Right motor calibration: %.2f\n", _rightCalibration);
}

float TankMotors::getLeftCalibration() const
{
    return _leftCalibration;
}

float TankMotors::getRightCalibration() const
{
    return _rightCalibration;
}

MotorDirection TankMotors::getLeftDirection() const
{
    return _leftDirection;
}

MotorDirection TankMotors::getRightDirection() const
{
    return _rightDirection;
}

uint8_t TankMotors::getLeftPower() const
{
    return _leftPower;
}

uint8_t TankMotors::getRightPower() const
{
    return _rightPower;
}

void TankMotors::applyLeftPower(uint8_t forwardPower, uint8_t backwardPower)
{
    analogWrite(_leftForwardPin, forwardPower);
    analogWrite(_leftBackwardPin, backwardPower);
}

void TankMotors::applyRightPower(uint8_t forwardPower, uint8_t backwardPower)
{
    analogWrite(_rightForwardPin, forwardPower);
    analogWrite(_rightBackwardPin, backwardPower);
}