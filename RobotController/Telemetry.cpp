#include "Telemetry.h"

// Initialize the static variables (these are shared by all instances of the class)
TelemetryData Telemetry::_data = {};
unsigned long Telemetry::_lastSendTime = 0;
unsigned long Telemetry::_interval = 1000;
bool Telemetry::_enabled = true;
bool Telemetry::_dataChanged = false;

/**
 * Start the telemetry system
 * 
 * This is like turning on the dashboard in a car - it gets everything
 * ready to show information about your robot.
 */
void Telemetry::begin(unsigned long interval) {
    _interval = interval;
    _lastSendTime = millis();
    _enabled = true;
    _dataChanged = false;
    
    // Set all values to zero or default values
    _data.leftMotorPower = 0;
    _data.rightMotorPower = 0;
    _data.leftMotorForward = true;
    _data.rightMotorForward = true;
    _data.leftCalibration = 1.0;
    _data.rightCalibration = 1.0;
    _data.batteryVoltage = 0.0;
    _data.isAccelerating = false;
    _data.smoothEnabled = true;
    _data.timestamp = 0;
}

/**
 * Update the telemetry data with new information
 * 
 * This is like updating the speedometer in a car when your speed changes.
 * We only update the data if something has actually changed, to avoid
 * sending the same information over and over.
 */
void Telemetry::update(uint8_t leftPower, uint8_t rightPower, 
                      bool leftForward, bool rightForward,
                      float leftCal, float rightCal,
                      bool accel, bool smooth) {
    // Check if any of the values have changed since last time
    if (_data.leftMotorPower != leftPower ||
        _data.rightMotorPower != rightPower ||
        _data.leftMotorForward != leftForward ||
        _data.rightMotorForward != rightForward ||
        _data.leftCalibration != leftCal ||
        _data.rightCalibration != rightCal ||
        _data.isAccelerating != accel ||
        _data.smoothEnabled != smooth) {
        
        // Update the data with new values
        _data.leftMotorPower = leftPower;
        _data.rightMotorPower = rightPower;
        _data.leftMotorForward = leftForward;
        _data.rightMotorForward = rightForward;
        _data.leftCalibration = leftCal;
        _data.rightCalibration = rightCal;
        _data.isAccelerating = accel;
        _data.smoothEnabled = smooth;
        _data.timestamp = millis();
        
        // Read battery voltage if available
        // This is just a placeholder - you would need to add a voltage sensor to your robot
        #ifdef BATTERY_PIN
        _data.batteryVoltage = analogRead(BATTERY_PIN) * (5.0 / 1023.0) * BATTERY_DIVIDER;
        #endif
        
        // Mark that the data has changed so we know to send it
        _dataChanged = true;
    }
}

/**
 * Send the telemetry data to the computer
 * 
 * This sends the robot's status information to your computer so you can see
 * what's happening. It only sends data when something has changed, to avoid
 * filling up your screen with repeated information.
 */
void Telemetry::send() {
    // Don't do anything if telemetry is turned off
    if (!_enabled) return;
    
    // Only send data if enough time has passed AND the data has changed
    unsigned long currentTime = millis();
    if ((currentTime - _lastSendTime >= _interval) && _dataChanged) {
        _lastSendTime = currentTime;
        _dataChanged = false;
        
        // Send the data as JSON (a format computers can easily understand)
        Serial.print("{");
        
        // Left motor information
        Serial.print("\"leftMotor\":{");
        Serial.print("\"power\":");
        Serial.print(_data.leftMotorPower);
        Serial.print(",\"forward\":");
        Serial.print(_data.leftMotorForward ? "true" : "false");
        Serial.print(",\"cal\":");
        Serial.print(_data.leftCalibration);
        Serial.print("},");
        
        // Right motor information
        Serial.print("\"rightMotor\":{");
        Serial.print("\"power\":");
        Serial.print(_data.rightMotorPower);
        Serial.print(",\"forward\":");
        Serial.print(_data.rightMotorForward ? "true" : "false");
        Serial.print(",\"cal\":");
        Serial.print(_data.rightCalibration);
        Serial.print("},");
        
        // System information
        Serial.print("\"system\":{");
        if (_data.batteryVoltage > 0) {
            Serial.print("\"battery\":");
            Serial.print(_data.batteryVoltage);
            Serial.print(",");
        }
        Serial.print("\"accel\":");
        Serial.print(_data.isAccelerating ? "true" : "false");
        Serial.print(",\"smooth\":");
        Serial.print(_data.smoothEnabled ? "true" : "false");
        Serial.print("},");
        
        // When this data was collected
        Serial.print("\"timestamp\":");
        Serial.print(_data.timestamp);
        Serial.println("}");
    }
}

/**
 * Get the current telemetry data
 * 
 * This lets other parts of the program see the current robot status.
 */
TelemetryData Telemetry::getData() {
    return _data;
}

/**
 * Change how often telemetry data is sent
 * 
 * This lets you adjust how frequently updates are sent to the computer.
 */
void Telemetry::setInterval(unsigned long interval) {
    _interval = interval;
}