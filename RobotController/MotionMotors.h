#ifndef MOTION_MOTORS_H
#define MOTION_MOTORS_H

#include <Arduino.h>
#include "Motor.h"
#include "Telemetry.h"

// Default settings
#define DEFAULT_SMOOTH_ENABLED true
#define SMOOTH_ACCEL_STEPS 10
#define SMOOTH_ACCEL_DELAY 20

// Acceleration parameters struct
struct AccelParams {
    uint8_t steps;
    uint8_t delayMs;
};

class MotionMotors {
public:
    // Constructor
    MotionMotors(uint8_t leftA, uint8_t leftB, uint8_t rightA, uint8_t rightB, 
                 float leftCalibration = 1.0, float rightCalibration = 1.0);
    
    // Initialize motors
    void begin();
    
    // Motor control functions
    void leftForward(uint8_t power, bool smooth = true);
    void leftBackward(uint8_t power, bool smooth = true);
    void rightForward(uint8_t power, bool smooth = true);
    void rightBackward(uint8_t power, bool smooth = true);
    void leftStop();
    void rightStop();
    void stop();
    
    // Smooth acceleration settings
    void setSmoothEnabled(bool enable);
    bool isSmoothEnabled();
    void updateAcceleration();
    
    // Calibration
    void setLeftCalibration(float calibration);
    void setRightCalibration(float calibration);
    float getLeftCalibration() const;
    float getRightCalibration() const;
    
    // Telemetry
    void updateTelemetry();
    
private:
    // Motors
    Motor _leftMotor;
    Motor _rightMotor;
    
    // Smooth acceleration
    bool _smoothEnabled;
    unsigned long _lastAccelUpdateTime;
    uint8_t _currentAccelStep;
    uint8_t _totalAccelSteps;
    bool _isAccelerating;
    
    // Target values for acceleration
    MotorDirection _leftTargetDirection;
    MotorDirection _rightTargetDirection;
    uint8_t _leftTargetPower;
    uint8_t _rightTargetPower;
    
    // Helper methods for smooth acceleration
    void startAcceleration(MotorDirection leftDir, uint8_t leftPower,
                          MotorDirection rightDir, uint8_t rightPower,
                          uint8_t steps);
    
    void smoothAccelerate(Motor& motor, MotorDirection direction, uint8_t targetPower,
                         const AccelParams& params = {SMOOTH_ACCEL_STEPS, SMOOTH_ACCEL_DELAY});
    
    void smoothDecelerate(Motor& motor, MotorDirection direction, uint8_t targetPower,
                         const AccelParams& params = {SMOOTH_ACCEL_STEPS, SMOOTH_ACCEL_DELAY});
    
    void smoothTransition(Motor& motor, MotorDirection newDirection, uint8_t targetPower,
                         const AccelParams& params = {SMOOTH_ACCEL_STEPS, SMOOTH_ACCEL_DELAY});
};

#endif // MOTION_MOTORS_H