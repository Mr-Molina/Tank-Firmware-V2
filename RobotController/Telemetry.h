#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>

/**
 * ROBOT TELEMETRY SYSTEM
 * 
 * This is like having a dashboard in a car that shows you important information
 * about how your robot is working. It sends data to your computer so you can see
 * what's happening with your robot in real-time!
 */

// This is all the information we want to know about our robot
struct TelemetryData {
    // Motor information - how fast and which direction the wheels are moving
    uint8_t leftMotorPower;     // How fast the left motor is going (0-255)
    uint8_t rightMotorPower;    // How fast the right motor is going (0-255)
    bool leftMotorForward;      // Is the left wheel going forward? (true/false)
    bool rightMotorForward;     // Is the right wheel going forward? (true/false)
    float leftCalibration;      // Left motor power adjustment (0.0-1.0)
    float rightCalibration;     // Right motor power adjustment (0.0-1.0)
    
    // System information - other important stuff about the robot
    float batteryVoltage;       // How much power is left in the battery
    
    // Status flags - what the robot is currently doing
    bool isAccelerating;        // Is the robot speeding up or slowing down?
    bool smoothEnabled;         // Is smooth acceleration turned on?
    
    // When this data was collected
    unsigned long timestamp;    // Time in milliseconds since the robot started
};

/**
 * The Telemetry class handles sending information about your robot
 * to your computer so you can see what's happening.
 */
class Telemetry {
public:
    /**
     * Start the telemetry system
     * 
     * @param interval How often to send data (in milliseconds)
     */
    static void begin(unsigned long interval = 1000);
    
    /**
     * Update the telemetry data with new information
     * 
     * @param leftPower Left motor speed (0-255)
     * @param rightPower Right motor speed (0-255)
     * @param leftForward Is left motor going forward?
     * @param rightForward Is right motor going forward?
     * @param leftCal Left motor calibration (0.0-1.0)
     * @param rightCal Right motor calibration (0.0-1.0)
     * @param accel Is the robot accelerating?
     * @param smooth Is smooth acceleration enabled?
     */
    static void update(uint8_t leftPower, uint8_t rightPower, 
                      bool leftForward, bool rightForward,
                      float leftCal, float rightCal,
                      bool accel, bool smooth);
    
    /**
     * Send the telemetry data to the computer
     * This only sends data when something has changed
     */
    static void send();
    
    /**
     * Get the current telemetry data
     * 
     * @return The current robot status information
     */
    static TelemetryData getData();
    
    /**
     * Change how often telemetry data is sent
     * 
     * @param interval Time between updates (in milliseconds)
     */
    static void setInterval(unsigned long interval);
    
private:
    // These variables are shared by all instances of the class
    static TelemetryData _data;         // The data we're collecting
    static unsigned long _lastSendTime; // When we last sent data
    static unsigned long _interval;     // How often to send data
    static bool _enabled;               // Is telemetry turned on?
    static bool _dataChanged;           // Has the data changed since last send?
};

#endif // TELEMETRY_H