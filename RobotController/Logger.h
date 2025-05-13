#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/**
 * DEBUG LEVELS
 * 
 * These control how much information your robot shares with you.
 * Think of it like choosing how chatty your robot should be!
 */
#define DEBUG_NONE 0       // Robot stays silent (no messages)
#define DEBUG_BASIC 1      // Robot tells you important things only
#define DEBUG_DETAILED 2   // Robot gives you more details
#define DEBUG_VERBOSE 3    // Robot tells you everything it's doing

/**
 * LOGGER CLASS
 * 
 * This is like your robot's voice! It helps your robot tell you
 * what it's doing and if there are any problems.
 */
class Logger {
public:
    /**
     * Start the logger
     * 
     * This gets the robot ready to talk to you
     */
    static void begin();
    
    /**
     * Set how much information the robot should share
     * 
     * @param level How chatty the robot should be (0-3)
     */
    static void setLevel(uint8_t level);
    
    /**
     * Check how chatty the robot is being
     * 
     * @return The current debug level (0-3)
     */
    static uint8_t getLevel();
    
    /**
     * Turn the robot's voice on or off
     * 
     * @param enabled true to let the robot talk, false to make it quiet
     */
    static void setEnabled(bool enabled);
    
    /**
     * Check if the robot is allowed to talk
     * 
     * @return true if the robot can talk, false if it's muted
     */
    static bool isEnabled();
    
    /**
     * Send a message from the robot
     * 
     * This is how the robot tells you what it's doing
     * 
     * @param level How important this message is (0-3)
     * @param format The message to send (with placeholders like %d for numbers)
     * @param ... The values to put in the placeholders
     */
    static void log(uint8_t level, const char* format, ...);
    
    /**
     * Send a message about the motors
     * 
     * This is a special function just for motor messages
     * 
     * @param motor Which motor ("Left" or "Right")
     * @param action What the motor is doing ("FORWARD", "BACKWARD", "STOP")
     * @param power How fast the motor is going (0-255)
     * @param calibratedPower The adjusted power after calibration
     */
    static void logMotor(const char* motor, const char* action, uint8_t power, uint8_t calibratedPower);
    
private:
    static uint8_t _level;   // How chatty the robot should be
    static bool _enabled;    // Is the robot allowed to talk?
};

// Global debug flags - these control what kinds of messages are shown
extern bool enableAccelerometerDebug;  // Show accelerometer data?
extern bool enableGyroscopeDebug;      // Show gyroscope data?
extern bool enableControllerDebug;     // Show controller input data?

#endif // LOGGER_H