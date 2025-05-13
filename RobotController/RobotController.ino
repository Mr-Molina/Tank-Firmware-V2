#include <Bluepad32.h>
#include <Preferences.h>
#include "MotionMotors.h"
#include "Logger.h"
#include "ControllerHandler.h"
#include "Telemetry.h"
#include "NonBlockingDelay.h"

/**
 * ROBOT CONTROLLER
 * 
 * This is the main program that controls your robot! It connects to a
 * game controller and uses it to drive the robot around.
 * 
 * The robot can:
 * - Drive forward and backward
 * - Turn left and right
 * - Adjust motor power for better balance
 * - Send information back to your computer
 * 
 * CONTROLLER BUTTON MAP:
 * ---------------------
 * Movement Controls:
 * - Left Joystick (Up/Down): Controls left motor (forward/backward)
 * - Right Joystick (Up/Down): Controls right motor (forward/backward)
 * 
 * Calibration Controls:
 * - A Button: Decrease right motor power by 5%
 * - Y Button: Increase right motor power by 5%
 * - D-pad UP: Increase left motor power by 5%
 * - D-pad DOWN: Decrease left motor power by 5%
 * 
 * Debug Controls:
 * - L1+R1 (pressed together): Cycle through debug levels (None → Basic → Detailed → Verbose)
 * - L2+R2 (pressed together): Toggle all debugging on/off
 * - X Button: Toggle controller debug messages on/off
 * - B Button: Toggle sensor debug messages on/off
 * 
 * Note: All debug messages are OFF by default. Use the X and B buttons to turn them on.
 */

// Create a preferences object to store settings even when power is off
Preferences preferences;

// This will store the single controller that can connect to the system
ControllerPtr connectedController = nullptr;

// Threshold values to filter out sensor jitter (ignore tiny movements)
const int gyroscopeJitterThreshold = 200;     // For rotation sensors
const int accelerometerJitterThreshold = 200; // For motion sensors

// Default calibration values for the motors
#define DEFAULT_LEFT_CALIBRATION 1.0   // Left motor at 100% power
#define DEFAULT_RIGHT_CALIBRATION 1.0  // Right motor at 100% power

// Create a global instance of the MotionMotors class to control the wheels
// Replace these pin numbers with your actual motor control pins
MotionMotors motors(18, 19, 16, 17);

// Structure to store the previous state of a controller
struct ControllerState {
    uint8_t dpadState;                  // D-pad buttons
    uint16_t buttonState;               // Other buttons
    int16_t leftJoystickX, leftJoystickY;   // Left stick position
    int16_t rightJoystickX, rightJoystickY; // Right stick position
    int16_t brakeValue, throttleValue;      // Trigger buttons
    int16_t gyroscopeX, gyroscopeY, gyroscopeZ;         // Rotation sensors
    int16_t accelerometerX, accelerometerY, accelerometerZ; // Motion sensors
};

// Store the previous state of the controller
ControllerState previousControllerState = {};

/**
 * Filter out small movements in controller sensors
 * 
 * This ignores tiny movements that might be accidental or from the
 * controller just sitting on a table.
 * 
 * @param value The sensor reading
 * @param threshold How big a movement needs to be to count
 * @return The filtered value (0 if below threshold)
 */
int filterJitter(int value, int threshold) {
    return (abs(value) > threshold) ? value : 0;
}

/**
 * This function is called when a new controller connects
 * 
 * It checks if we already have a controller connected. If not,
 * it stores the new controller and prints information about it.
 * If we already have a controller, it ignores the new one.
 */
void onConnectedController(ControllerPtr controller) {
    // If we already have a controller connected, ignore this one
    if (connectedController != nullptr) {
        Logger::log(DEBUG_BASIC, "New controller ignored - only one controller allowed");
        return;
    }
    
    // Store the new controller
    connectedController = controller;
    
    // Print information about the controller
    Logger::log(DEBUG_BASIC, "Controller connected!");
    
    ControllerProperties properties = controller->getProperties();
    Logger::log(DEBUG_DETAILED, "Controller model: %s, VID=0x%04x, PID=0x%04x", 
               controller->getModelName().c_str(), properties.vendor_id, properties.product_id);
}

/**
 * This function is called when a controller disconnects
 * 
 * It clears our controller variable and prints a message.
 */
void onDisconnectedController(ControllerPtr controller) {
    // Check if this is our connected controller
    if (connectedController == controller) {
        Logger::log(DEBUG_BASIC, "Controller disconnected");
        connectedController = nullptr;
        
        // Stop the motors for safety when controller disconnects
        motors.stop();
    }
}

/**
 * This function prints detailed information about the connected gamepad
 * 
 * It shows all the buttons, joysticks, and sensors on the controller,
 * but only when something changes to avoid flooding the screen.
 */
void dumpGamepad(ControllerPtr controller) {
    if (!controller) return;
    
    // Get the current state of the controller
    uint8_t dpadState = controller->dpad();
    uint16_t buttonState = controller->buttons();
    int16_t leftJoystickX = controller->axisX();
    int16_t leftJoystickY = controller->axisY();
    int16_t rightJoystickX = controller->axisRX();
    int16_t rightJoystickY = controller->axisRY();
    int16_t brakeValue = controller->brake();
    int16_t throttleValue = controller->throttle();
    int16_t gyroscopeX = filterJitter(controller->gyroX(), gyroscopeJitterThreshold);
    int16_t gyroscopeY = filterJitter(controller->gyroY(), gyroscopeJitterThreshold);
    int16_t gyroscopeZ = filterJitter(controller->gyroZ(), gyroscopeJitterThreshold);
    int16_t accelerometerX = filterJitter(controller->accelX(), accelerometerJitterThreshold);
    int16_t accelerometerY = filterJitter(controller->accelY(), accelerometerJitterThreshold);
    int16_t accelerometerZ = filterJitter(controller->accelZ(), accelerometerJitterThreshold);
    
    // Check if the state has changed
    if (dpadState != previousControllerState.dpadState || buttonState != previousControllerState.buttonState ||
        leftJoystickX != previousControllerState.leftJoystickX || leftJoystickY != previousControllerState.leftJoystickY ||
        rightJoystickX != previousControllerState.rightJoystickX || rightJoystickY != previousControllerState.rightJoystickY ||
        brakeValue != previousControllerState.brakeValue || throttleValue != previousControllerState.throttleValue ||
        (enableGyroscopeDebug && (gyroscopeX != previousControllerState.gyroscopeX || gyroscopeY != previousControllerState.gyroscopeY || gyroscopeZ != previousControllerState.gyroscopeZ)) ||
        (enableAccelerometerDebug && (accelerometerX != previousControllerState.accelerometerX || accelerometerY != previousControllerState.accelerometerY || accelerometerZ != previousControllerState.accelerometerZ))) {
        
        // Print the debug information if enabled
        if (Logger::isEnabled() && enableControllerDebug) {
            Serial.printf(
                "dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d",
                dpadState, buttonState, leftJoystickX, leftJoystickY, rightJoystickX, rightJoystickY, brakeValue, throttleValue);
            
            if (enableGyroscopeDebug) {
                Serial.printf(", gyro x:%6d y:%6d z:%6d", gyroscopeX, gyroscopeY, gyroscopeZ);
            }
            
            if (enableAccelerometerDebug) {
                Serial.printf(", accel x:%6d y:%6d z:%6d", accelerometerX, accelerometerY, accelerometerZ);
            }
            
            Serial.println();
        }
        
        // Update the previous state
        previousControllerState = {dpadState, buttonState, leftJoystickX, leftJoystickY, rightJoystickX, rightJoystickY, brakeValue, throttleValue, gyroscopeX, gyroscopeY, gyroscopeZ, accelerometerX, accelerometerY, accelerometerZ};
    }
}

/**
 * This function processes the connected controller
 * 
 * It checks if the controller is connected and handles its input.
 */
void processController() {
    if (connectedController && connectedController->isConnected() && connectedController->hasData()) {
        if (connectedController->isGamepad()) {
            ControllerHandler::processController(connectedController);
            dumpGamepad(connectedController);
        } else {
            Logger::log(DEBUG_BASIC, "Unsupported controller type");
        }
    }
}

/**
 * This function runs once when the Arduino starts
 * 
 * It sets up everything the robot needs to work properly.
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    
    // Initialize the logger
    Logger::begin();
    Logger::setLevel(DEBUG_BASIC);
    
    // Initialize telemetry
    Telemetry::begin(1000); // Send telemetry every 1000ms
    
    // Print firmware information
    Logger::log(DEBUG_BASIC, "Firmware: %s", BP32.firmwareVersion());
    const uint8_t *addr = BP32.localBdAddress();
    Logger::log(DEBUG_BASIC, "BD Addr: %2X:%2X:%2X:%2X:%2X:%2X", 
               addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    
    // Set up the Bluepad32 library
    BP32.setup(&onConnectedController, &onDisconnectedController);
    
    // Forget all previously paired Bluetooth devices (optional)
    BP32.forgetBluetoothKeys();
    
    // Disable virtual devices (e.g., mouse emulation for gamepads)
    BP32.enableVirtualDevice(false);
    
    // Initialize preferences for persistent storage
    preferences.begin("robot", false); // "robot" is the namespace, false means RW mode
    
    // Load saved calibration values or use defaults
    float leftCal = constrain(preferences.getFloat("leftCal", DEFAULT_LEFT_CALIBRATION), 0.0, 1.0);
    float rightCal = constrain(preferences.getFloat("rightCal", DEFAULT_RIGHT_CALIBRATION), 0.0, 1.0);
    
    // Initialize the motors with saved calibration values
    motors.begin();
    motors.setLeftCalibration(leftCal);
    motors.setRightCalibration(rightCal);
    
    // Initialize the controller handler
    ControllerHandler::begin(motors, preferences);
    
    // Print loaded calibration values
    Logger::log(DEBUG_BASIC, "Loaded calibration - Left: %.2f, Right: %.2f", leftCal, rightCal);
    
    // Print debug control instructions
    Logger::log(DEBUG_BASIC, "Debug controls:");
    Logger::log(DEBUG_BASIC, "- L1+R1: Cycle debug levels");
    Logger::log(DEBUG_BASIC, "- L2+R2: Toggle all debugging on/off");
    Logger::log(DEBUG_BASIC, "- X: Toggle controller debug messages (currently OFF)");
    Logger::log(DEBUG_BASIC, "- B: Toggle sensor debug messages (currently OFF)");
    
    // Print single controller mode message
    Logger::log(DEBUG_BASIC, "Single controller mode: Only one controller can connect at a time");
}

/**
 * This function runs repeatedly in a loop
 * 
 * It's the main part of the program that keeps checking for controller
 * input and updating the motors.
 */
void loop() {
    // Update Bluepad32 and process controller
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processController();
    }
    
    // Update motor acceleration for smooth speed changes
    motors.updateAcceleration();
    
    // Send telemetry data only when it changes
    static unsigned long lastTelemetryCheck = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastTelemetryCheck >= 100) { // Check every 100ms
        lastTelemetryCheck = currentTime;
        Telemetry::send();
    }
}