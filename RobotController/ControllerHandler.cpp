#include "ControllerHandler.h"

// Initialize static members
MotionMotors* ControllerHandler::_motors = nullptr;
Preferences* ControllerHandler::_prefs = nullptr;
unsigned long ControllerHandler::_lastButtonPressTime = 0;

/**
 * Initialize the controller handler
 * 
 * This gets everything ready to process controller input.
 */
void ControllerHandler::begin(MotionMotors& motors, Preferences& prefs) {
    _motors = &motors;
    _prefs = &prefs;
}

/**
 * Process controller input
 * 
 * This reads the controller and makes the robot respond.
 * It handles different aspects of the controller separately.
 */
void ControllerHandler::processController(ControllerPtr controller) {
    if (!controller) return;
    
    // Handle different aspects of controller input
    handleMovement(controller);
    handleCalibrationButtons(controller);
    handleDebugButtons(controller);
}

/**
 * Handle movement controls (joysticks)
 * 
 * This makes the robot drive based on joystick positions:
 * - Left joystick controls the left motor
 * - Right joystick controls the right motor
 * - Up makes the motor go forward
 * - Down makes the motor go backward
 */
void ControllerHandler::handleMovement(ControllerPtr controller) {
    // Get the joystick Y-axis values
    int16_t leftJoystickY = controller->axisY();   // Left joystick Y-axis
    int16_t rightJoystickY = controller->axisRY(); // Right joystick Y-axis

    // Apply dead zone to joystick values (ignore small movements)
    if (abs(leftJoystickY) < JOYSTICK_DEAD_ZONE) {
        leftJoystickY = 0;
    }
    if (abs(rightJoystickY) < JOYSTICK_DEAD_ZONE) {
        rightJoystickY = 0;
    }

    // Map joystick values (-32768 to 32767) to motor power (0 to 255)
    int leftMotorPower = map(abs(leftJoystickY), 0, 32767, 0, 255);
    int rightMotorPower = map(abs(rightJoystickY), 0, 32767, 0, 255);

    // Control the left motor based on the joystick direction
    if (leftJoystickY > 0) {
        _motors->leftForward(leftMotorPower); // Move forward
    } else if (leftJoystickY < 0) {
        _motors->leftBackward(leftMotorPower); // Move backward
    } else {
        _motors->leftStop(); // Stop the motor
    }

    // Control the right motor based on the joystick direction
    if (rightJoystickY > 0) {
        _motors->rightForward(rightMotorPower); // Move forward
    } else if (rightJoystickY < 0) {
        _motors->rightBackward(rightMotorPower); // Move backward
    } else {
        _motors->rightStop(); // Stop the motor
    }
}

/**
 * Handle calibration buttons
 * 
 * This adjusts motor power balance when buttons are pressed:
 * - A button: Decrease right motor power
 * - Y button: Increase right motor power
 * - D-pad UP: Increase left motor power
 * - D-pad DOWN: Decrease left motor power
 */
void ControllerHandler::handleCalibrationButtons(ControllerPtr controller) {
    if (!canProcessButton()) return;
    
    // A button - Decrease right motor calibration
    if (controller->a()) {
        float newCalibration = constrain(_motors->getRightCalibration() - CALIBRATION_STEP, 0.0, 1.0);
        _motors->setRightCalibration(newCalibration);
        _prefs->putFloat("rightCal", newCalibration); // Save to persistent storage
        _lastButtonPressTime = millis();
    }
    
    // Y button - Increase right motor calibration
    else if (controller->y()) {
        float newCalibration = constrain(_motors->getRightCalibration() + CALIBRATION_STEP, 0.0, 1.0);
        _motors->setRightCalibration(newCalibration);
        _prefs->putFloat("rightCal", newCalibration); // Save to persistent storage
        _lastButtonPressTime = millis();
    }
    
    // D-pad UP - Increase left motor calibration
    else if (controller->dpad() == DPAD_UP) {
        float newCalibration = constrain(_motors->getLeftCalibration() + CALIBRATION_STEP, 0.0, 1.0);
        _motors->setLeftCalibration(newCalibration);
        _prefs->putFloat("leftCal", newCalibration); // Save to persistent storage
        _lastButtonPressTime = millis();
    }
    
    // D-pad DOWN - Decrease left motor calibration
    else if (controller->dpad() == DPAD_DOWN) {
        float newCalibration = constrain(_motors->getLeftCalibration() - CALIBRATION_STEP, 0.0, 1.0);
        _motors->setLeftCalibration(newCalibration);
        _prefs->putFloat("leftCal", newCalibration); // Save to persistent storage
        _lastButtonPressTime = millis();
    }
}

/**
 * Handle debug buttons
 * 
 * This changes what information the robot shows when buttons are pressed:
 * - L1+R1: Cycle through debug levels (None → Basic → Detailed → Verbose)
 * - L2+R2: Toggle all debugging on/off
 * - X button: Toggle controller debug messages
 * - B button: Toggle sensor debug messages
 */
void ControllerHandler::handleDebugButtons(ControllerPtr controller) {
    if (!canProcessButton()) return;
    
    // L1+R1 - Cycle through motor debug levels
    if (controller->l1() && controller->r1()) {
        uint8_t newLevel = (Logger::getLevel() + 1) % 4;
        Logger::setLevel(newLevel);
        Logger::log(DEBUG_BASIC, "Debug level set to: %d (%s)", 
                   newLevel,
                   newLevel == DEBUG_NONE ? "None" :
                   newLevel == DEBUG_BASIC ? "Basic" :
                   newLevel == DEBUG_DETAILED ? "Detailed" : "Verbose");
        _lastButtonPressTime = millis();
    }
    
    // L2+R2 - Toggle all debugging
    else if (controller->l2() && controller->r2()) {
        bool enabled = !Logger::isEnabled();
        Logger::setEnabled(enabled);
        Serial.printf("All debugging %s\n", enabled ? "ENABLED" : "DISABLED");
        _lastButtonPressTime = millis();
    }
    
    // X button - Toggle controller debug
    else if (controller->x()) {
        enableControllerDebug = !enableControllerDebug;
        Logger::log(DEBUG_BASIC, "Controller debugging %s", 
                   enableControllerDebug ? "ENABLED" : "DISABLED");
        _lastButtonPressTime = millis();
    }
    
    // B button - Toggle sensor debug
    else if (controller->b()) {
        enableAccelerometerDebug = !enableAccelerometerDebug;
        enableGyroscopeDebug = !enableGyroscopeDebug;
        Logger::log(DEBUG_BASIC, "Sensor debugging %s", 
                   enableGyroscopeDebug ? "ENABLED" : "DISABLED");
        _lastButtonPressTime = millis();
    }
}

/**
 * Check if a button can be processed
 * 
 * This prevents multiple actions from a single button press by
 * making sure enough time has passed since the last button press.
 */
bool ControllerHandler::canProcessButton() {
    return (millis() - _lastButtonPressTime > DEBOUNCE_DELAY);
}