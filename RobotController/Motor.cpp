#include "Motor.h"

Motor::Motor(uint8_t forwardPin, uint8_t backwardPin, float calibration) {
    _forwardPin = forwardPin;
    _backwardPin = backwardPin;
    _calibration = constrain(calibration, 0.0, 1.0);
    _direction = MOTOR_STOPPED;
    _power = 0;
    _lastReportedDirection = MOTOR_STOPPED;
    _lastReportedPower = 0;
}

void Motor::begin() {
    pinMode(_forwardPin, OUTPUT);
    pinMode(_backwardPin, OUTPUT);
    stop();
}

void Motor::forward(uint8_t power) {
    _direction = MOTOR_FORWARD;
    _power = power;
    
    uint8_t calibratedPower = power * _calibration;
    applyPower(calibratedPower, 0);
    
    // Log if state changed
    if (_lastReportedDirection != _direction || _lastReportedPower != _power) {
        Logger::logMotor("", "FORWARD", power, calibratedPower);
        _lastReportedDirection = _direction;
        _lastReportedPower = _power;
    }
}

void Motor::backward(uint8_t power) {
    _direction = MOTOR_BACKWARD;
    _power = power;
    
    uint8_t calibratedPower = power * _calibration;
    applyPower(0, calibratedPower);
    
    // Log if state changed
    if (_lastReportedDirection != _direction || _lastReportedPower != _power) {
        Logger::logMotor("", "BACKWARD", power, calibratedPower);
        _lastReportedDirection = _direction;
        _lastReportedPower = _power;
    }
}

void Motor::stop() {
    _direction = MOTOR_STOPPED;
    _power = 0;
    
    applyPower(0, 0);
    
    // Log if state changed
    if (_lastReportedDirection != _direction) {
        Logger::logMotor("", "STOP", 0, 0);
        _lastReportedDirection = _direction;
        _lastReportedPower = _power;
    }
}

MotorDirection Motor::getDirection() const {
    return _direction;
}

uint8_t Motor::getPower() const {
    return _power;
}

void Motor::setCalibration(float calibration) {
    _calibration = constrain(calibration, 0.0, 1.0);
}

float Motor::getCalibration() const {
    return _calibration;
}

void Motor::applyPower(uint8_t forwardPower, uint8_t backwardPower) {
    analogWrite(_forwardPin, forwardPower);
    analogWrite(_backwardPin, backwardPower);
}