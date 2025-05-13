#include "Logger.h"

// Initialize static variables
uint8_t Logger::_level = DEBUG_BASIC;
bool Logger::_enabled = true;

// Global debug flags - all set to false by default
bool enableAccelerometerDebug = false;
bool enableGyroscopeDebug = false;
bool enableControllerDebug = false;

/**
 * Start the logger
 * 
 * This gets everything ready for the robot to send messages.
 */
void Logger::begin() {
    // Nothing to initialize
}

/**
 * Set how much information the robot should share
 * 
 * This is like adjusting the volume on your robot's voice:
 * - Level 0 (NONE): Robot stays completely silent
 * - Level 1 (BASIC): Robot tells you important things only
 * - Level 2 (DETAILED): Robot gives you more details
 * - Level 3 (VERBOSE): Robot tells you everything it's doing
 */
void Logger::setLevel(uint8_t level) {
    _level = level;
}

/**
 * Check how chatty the robot is being
 * 
 * This tells you the current "volume" of your robot's voice.
 */
uint8_t Logger::getLevel() {
    return _level;
}

/**
 * Turn the robot's voice on or off
 * 
 * This is like a mute button for your robot.
 */
void Logger::setEnabled(bool enabled) {
    _enabled = enabled;
    
    // If we're turning off logging, set level to none
    if (!enabled) {
        _level = DEBUG_NONE;
    }
}

/**
 * Check if the robot is allowed to talk
 * 
 * This tells you if the robot's voice is turned on or off.
 */
bool Logger::isEnabled() {
    return _enabled;
}

/**
 * Send a message from the robot
 * 
 * This is how the robot tells you what it's doing. The robot will only
 * send the message if it's important enough (level is less than or equal
 * to the current debug level).
 */
void Logger::log(uint8_t level, const char* format, ...) {
    // Only log if enabled and level is sufficient
    if (_enabled && level <= _level) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        Serial.println(buffer);
        va_end(args);
    }
}

/**
 * Send a message about the motors
 * 
 * This is a special function just for motor messages. It formats the
 * message in a consistent way and includes power information.
 */
void Logger::logMotor(const char* motor, const char* action, uint8_t power, uint8_t calibratedPower) {
    if (_enabled && _level >= DEBUG_BASIC) {
        Serial.printf("%s motor: %s at power %d", motor, action, power);
        
        if (_level >= DEBUG_DETAILED) {
            Serial.printf(" (calibrated: %d)", calibratedPower);
        }
        
        Serial.println();
    }
}