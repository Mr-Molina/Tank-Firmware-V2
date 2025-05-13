#include "MotionMotors.h"
#include "Logger.h"

MotionMotors::MotionMotors(uint8_t leftA, uint8_t leftB, uint8_t rightA, uint8_t rightB,
                         float leftCalibration, float rightCalibration)
    : _leftMotor(leftA, leftB, leftCalibration),
      _rightMotor(rightA, rightB, rightCalibration) {
    
    _smoothEnabled = DEFAULT_SMOOTH_ENABLED;
    _lastAccelUpdateTime = 0;
    _currentAccelStep = 0;
    _totalAccelSteps = 0;
    _isAccelerating = false;
    _leftTargetDirection = MOTOR_STOPPED;
    _rightTargetDirection = MOTOR_STOPPED;
    _leftTargetPower = 0;
    _rightTargetPower = 0;
}

void MotionMotors::begin() {
    _leftMotor.begin();
    _rightMotor.begin();
    Logger::log(DEBUG_BASIC, "MotionMotors initialized");
    
    // Initialize telemetry
    Telemetry::begin();
    updateTelemetry();
}

void MotionMotors::leftForward(uint8_t power, bool smooth) {
    if (smooth && _smoothEnabled) {
        if (_leftMotor.getDirection() == MOTOR_FORWARD) {
            // Already moving in this direction, adjust speed
            if (power > _leftMotor.getPower()) {
                smoothAccelerate(_leftMotor, MOTOR_FORWARD, power);
            } else if (power < _leftMotor.getPower()) {
                smoothDecelerate(_leftMotor, MOTOR_FORWARD, power);
            }
        } else {
            // Changing direction or starting from stop
            smoothTransition(_leftMotor, MOTOR_FORWARD, power);
        }
    } else {
        // Immediate change
        _leftMotor.forward(power);
    }
    updateTelemetry();
}

void MotionMotors::leftBackward(uint8_t power, bool smooth) {
    if (smooth && _smoothEnabled) {
        if (_leftMotor.getDirection() == MOTOR_BACKWARD) {
            // Already moving in this direction, adjust speed
            if (power > _leftMotor.getPower()) {
                smoothAccelerate(_leftMotor, MOTOR_BACKWARD, power);
            } else if (power < _leftMotor.getPower()) {
                smoothDecelerate(_leftMotor, MOTOR_BACKWARD, power);
            }
        } else {
            // Changing direction or starting from stop
            smoothTransition(_leftMotor, MOTOR_BACKWARD, power);
        }
    } else {
        // Immediate change
        _leftMotor.backward(power);
    }
    updateTelemetry();
}

void MotionMotors::rightForward(uint8_t power, bool smooth) {
    if (smooth && _smoothEnabled) {
        if (_rightMotor.getDirection() == MOTOR_FORWARD) {
            // Already moving in this direction, adjust speed
            if (power > _rightMotor.getPower()) {
                smoothAccelerate(_rightMotor, MOTOR_FORWARD, power);
            } else if (power < _rightMotor.getPower()) {
                smoothDecelerate(_rightMotor, MOTOR_FORWARD, power);
            }
        } else {
            // Changing direction or starting from stop
            smoothTransition(_rightMotor, MOTOR_FORWARD, power);
        }
    } else {
        // Immediate change
        _rightMotor.forward(power);
    }
    updateTelemetry();
}

void MotionMotors::rightBackward(uint8_t power, bool smooth) {
    if (smooth && _smoothEnabled) {
        if (_rightMotor.getDirection() == MOTOR_BACKWARD) {
            // Already moving in this direction, adjust speed
            if (power > _rightMotor.getPower()) {
                smoothAccelerate(_rightMotor, MOTOR_BACKWARD, power);
            } else if (power < _rightMotor.getPower()) {
                smoothDecelerate(_rightMotor, MOTOR_BACKWARD, power);
            }
        } else {
            // Changing direction or starting from stop
            smoothTransition(_rightMotor, MOTOR_BACKWARD, power);
        }
    } else {
        // Immediate change
        _rightMotor.backward(power);
    }
    updateTelemetry();
}

void MotionMotors::leftStop() {
    if (_smoothEnabled && _leftMotor.getPower() > 0) {
        // Gradually slow down
        smoothDecelerate(_leftMotor, _leftMotor.getDirection(), 0);
    } else {
        // Immediate stop
        _leftMotor.stop();
    }
    updateTelemetry();
}

void MotionMotors::rightStop() {
    if (_smoothEnabled && _rightMotor.getPower() > 0) {
        // Gradually slow down
        smoothDecelerate(_rightMotor, _rightMotor.getDirection(), 0);
    } else {
        // Immediate stop
        _rightMotor.stop();
    }
    updateTelemetry();
}

void MotionMotors::stop() {
    Logger::log(DEBUG_VERBOSE, "Stopping all motors");
    leftStop();
    rightStop();
    _isAccelerating = false;
    Logger::log(DEBUG_BASIC, "All motors stopped");
    updateTelemetry();
}

void MotionMotors::setSmoothEnabled(bool enable) {
    _smoothEnabled = enable;
    Logger::log(DEBUG_BASIC, "Smooth acceleration %s", enable ? "ENABLED" : "DISABLED");
    updateTelemetry();
}

bool MotionMotors::isSmoothEnabled() {
    return _smoothEnabled;
}

void MotionMotors::setLeftCalibration(float calibration) {
    float oldCalibration = _leftMotor.getCalibration();
    _leftMotor.setCalibration(calibration);
    Logger::log(DEBUG_DETAILED, "Left motor calibration changed: %.2f -> %.2f", 
               oldCalibration, _leftMotor.getCalibration());
    updateTelemetry();
}

void MotionMotors::setRightCalibration(float calibration) {
    float oldCalibration = _rightMotor.getCalibration();
    _rightMotor.setCalibration(calibration);
    Logger::log(DEBUG_DETAILED, "Right motor calibration changed: %.2f -> %.2f", 
               oldCalibration, _rightMotor.getCalibration());
    updateTelemetry();
}

float MotionMotors::getLeftCalibration() const {
    return _leftMotor.getCalibration();
}

float MotionMotors::getRightCalibration() const {
    return _rightMotor.getCalibration();
}

void MotionMotors::updateAcceleration() {
    if (!_isAccelerating || !_smoothEnabled) {
        return;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - _lastAccelUpdateTime < SMOOTH_ACCEL_DELAY) {
        return;
    }
    
    _lastAccelUpdateTime = currentTime;
    _currentAccelStep++;
    
    Logger::log(DEBUG_VERBOSE, "Acceleration step %d/%d (%.1f%%)", 
               _currentAccelStep, _totalAccelSteps, 
               (float)_currentAccelStep / _totalAccelSteps * 100);
    
    if (_currentAccelStep >= _totalAccelSteps) {
        // We've reached the target
        if (_leftTargetDirection != MOTOR_STOPPED) {
            if (_leftTargetDirection == MOTOR_FORWARD) {
                _leftMotor.forward(_leftTargetPower);
            } else {
                _leftMotor.backward(_leftTargetPower);
            }
            Logger::log(DEBUG_VERBOSE, "Left motor acceleration complete. Final power: %d", _leftTargetPower);
        }
        
        if (_rightTargetDirection != MOTOR_STOPPED) {
            if (_rightTargetDirection == MOTOR_FORWARD) {
                _rightMotor.forward(_rightTargetPower);
            } else {
                _rightMotor.backward(_rightTargetPower);
            }
            Logger::log(DEBUG_VERBOSE, "Right motor acceleration complete. Final power: %d", _rightTargetPower);
        }
        
        _isAccelerating = false;
        updateTelemetry();
        return;
    }
    
    // Calculate intermediate power
    float progress = (float)_currentAccelStep / _totalAccelSteps;
    
    if (_leftTargetDirection != MOTOR_STOPPED) {
        uint8_t power = _leftTargetPower * progress;
        if (_leftTargetDirection == MOTOR_FORWARD) {
            _leftMotor.forward(power);
        } else {
            _leftMotor.backward(power);
        }
    }
    
    if (_rightTargetDirection != MOTOR_STOPPED) {
        uint8_t power = _rightTargetPower * progress;
        if (_rightTargetDirection == MOTOR_FORWARD) {
            _rightMotor.forward(power);
        } else {
            _rightMotor.backward(power);
        }
    }
    
    // Update telemetry periodically during acceleration
    if (_currentAccelStep % 3 == 0) {
        updateTelemetry();
    }
}

void MotionMotors::startAcceleration(MotorDirection leftDir, uint8_t leftPower,
                                   MotorDirection rightDir, uint8_t rightPower,
                                   uint8_t steps) {
    _leftTargetDirection = leftDir;
    _rightTargetDirection = rightDir;
    _leftTargetPower = leftPower;
    _rightTargetPower = rightPower;
    _currentAccelStep = 0;
    _totalAccelSteps = steps;
    _lastAccelUpdateTime = millis();
    _isAccelerating = true;
    
    Logger::log(DEBUG_VERBOSE, "Starting acceleration sequence");
    updateTelemetry();
}

void MotionMotors::smoothAccelerate(Motor& motor, MotorDirection direction, uint8_t targetPower,
                                  const AccelParams& params) {
    if (!_smoothEnabled) {
        // Apply immediately if smooth acceleration is disabled
        if (direction == MOTOR_FORWARD) {
            motor.forward(targetPower);
        } else if (direction == MOTOR_BACKWARD) {
            motor.backward(targetPower);
        }
        updateTelemetry();
        return;
    }
    
    // Set up acceleration for this motor
    if (&motor == &_leftMotor) {
        _leftTargetDirection = direction;
        _leftTargetPower = targetPower;
        _rightTargetDirection = MOTOR_STOPPED;
        _rightTargetPower = 0;
    } else {
        _leftTargetDirection = MOTOR_STOPPED;
        _leftTargetPower = 0;
        _rightTargetDirection = direction;
        _rightTargetPower = targetPower;
    }
    
    _currentAccelStep = 0;
    _totalAccelSteps = params.steps;
    _lastAccelUpdateTime = millis();
    _isAccelerating = true;
    updateTelemetry();
}

void MotionMotors::smoothDecelerate(Motor& motor, MotorDirection direction, uint8_t targetPower,
                                  const AccelParams& params) {
    if (!_smoothEnabled) {
        // Apply immediately if smooth deceleration is disabled
        if (targetPower == 0) {
            motor.stop();
        } else if (direction == MOTOR_FORWARD) {
            motor.forward(targetPower);
        } else if (direction == MOTOR_BACKWARD) {
            motor.backward(targetPower);
        }
        updateTelemetry();
        return;
    }
    
    // Set up deceleration for this motor
    if (&motor == &_leftMotor) {
        _leftTargetDirection = (targetPower == 0) ? MOTOR_STOPPED : direction;
        _leftTargetPower = targetPower;
        _rightTargetDirection = MOTOR_STOPPED;
        _rightTargetPower = 0;
    } else {
        _leftTargetDirection = MOTOR_STOPPED;
        _leftTargetPower = 0;
        _rightTargetDirection = (targetPower == 0) ? MOTOR_STOPPED : direction;
        _rightTargetPower = targetPower;
    }
    
    _currentAccelStep = 0;
    _totalAccelSteps = params.steps;
    _lastAccelUpdateTime = millis();
    _isAccelerating = true;
    updateTelemetry();
}

void MotionMotors::smoothTransition(Motor& motor, MotorDirection newDirection, uint8_t targetPower,
                                  const AccelParams& params) {
    if (!_smoothEnabled) {
        // Apply immediately if smooth transition is disabled
        if (newDirection == MOTOR_FORWARD) {
            motor.forward(targetPower);
        } else if (newDirection == MOTOR_BACKWARD) {
            motor.backward(targetPower);
        } else {
            motor.stop();
        }
        updateTelemetry();
        return;
    }
    
    // If changing direction, stop first
    if (motor.getDirection() != MOTOR_STOPPED && motor.getDirection() != newDirection) {
        motor.stop();
        // Non-blocking approach: we'll add an extra step at the beginning with zero power
        // This effectively creates a pause without using delay()
        _currentAccelStep = 0;
        _totalAccelSteps += 2; // Add extra steps for the pause
    }
    
    // Set up transition for this motor
    if (&motor == &_leftMotor) {
        _leftTargetDirection = newDirection;
        _leftTargetPower = targetPower;
        _rightTargetDirection = MOTOR_STOPPED;
        _rightTargetPower = 0;
    } else {
        _leftTargetDirection = MOTOR_STOPPED;
        _leftTargetPower = 0;
        _rightTargetDirection = newDirection;
        _rightTargetPower = targetPower;
    }
    
    _currentAccelStep = 0;
    _totalAccelSteps = params.steps;
    _lastAccelUpdateTime = millis();
    _isAccelerating = true;
    updateTelemetry();
}

void MotionMotors::updateTelemetry() {
    // Only update telemetry when motor state changes
    static uint8_t lastLeftPower = 0;
    static uint8_t lastRightPower = 0;
    static MotorDirection lastLeftDir = MOTOR_STOPPED;
    static MotorDirection lastRightDir = MOTOR_STOPPED;
    static float lastLeftCal = 0;
    static float lastRightCal = 0;
    static bool lastAccel = false;
    static bool lastSmooth = true;
    
    // Get current state
    uint8_t leftPower = _leftMotor.getPower();
    uint8_t rightPower = _rightMotor.getPower();
    MotorDirection leftDir = _leftMotor.getDirection();
    MotorDirection rightDir = _rightMotor.getDirection();
    float leftCal = _leftMotor.getCalibration();
    float rightCal = _rightMotor.getCalibration();
    
    // Check if anything changed
    if (leftPower != lastLeftPower || rightPower != lastRightPower ||
        leftDir != lastLeftDir || rightDir != lastRightDir ||
        leftCal != lastLeftCal || rightCal != lastRightCal ||
        _isAccelerating != lastAccel || _smoothEnabled != lastSmooth) {
        
        // Update telemetry
        bool leftForward = (leftDir == MOTOR_FORWARD);
        bool rightForward = (rightDir == MOTOR_FORWARD);
        
        Telemetry::update(
            leftPower,
            rightPower,
            leftForward,
            rightForward,
            leftCal,
            rightCal,
            _isAccelerating,
            _smoothEnabled
        );
        
        // Save current state
        lastLeftPower = leftPower;
        lastRightPower = rightPower;
        lastLeftDir = leftDir;
        lastRightDir = rightDir;
        lastLeftCal = leftCal;
        lastRightCal = rightCal;
        lastAccel = _isAccelerating;
        lastSmooth = _smoothEnabled;
    }
}