#include <Bluepad32.h>
#include <Preferences.h>
#include "TankMotors.h"

/**
 * ROBOT CONTROLLER
 *
 * This is the main program that controls your robot using a game controller.
 */

// Create a preferences object to store settings
Preferences preferences;

// This will store the single controller that can connect to the system
ControllerPtr connectedController = nullptr;

// Pin definitions
#define LEFT_FORWARD_PIN 33
#define LEFT_BACKWARD_PIN 32
#define RIGHT_FORWARD_PIN 25
#define RIGHT_BACKWARD_PIN 26

// Create a global instance of the TankMotors class
TankMotors motors(LEFT_FORWARD_PIN, LEFT_BACKWARD_PIN, RIGHT_FORWARD_PIN, RIGHT_BACKWARD_PIN);

// Button debounce settings
#define DEBOUNCE_DELAY 300
unsigned long lastButtonPressTime = 0;

// Joystick settings
#define JOYSTICK_DEAD_ZONE 20

// Calibration step size
#define CALIBRATION_STEP 0.05f

/**
 * This function is called when a new controller connects
 */
void onConnectedController(ControllerPtr controller)
{
    if (connectedController != nullptr)
    {
        Serial.println("New controller ignored - only one controller allowed");
        return;
    }

    connectedController = controller;
    Serial.println("Controller connected!");

    ControllerProperties properties = controller->getProperties();
    Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n",
                  controller->getModelName().c_str(), properties.vendor_id, properties.product_id);
}

/**
 * This function is called when a controller disconnects
 */
void onDisconnectedController(ControllerPtr controller)
{
    if (connectedController == controller)
    {
        Serial.println("Controller disconnected");
        connectedController = nullptr;

        // Stop the motors for safety when controller disconnects
        motors.stop();
    }
}

/**
 * Handle movement controls (joysticks)
 */
void handleMovement(ControllerPtr controller)
{
    // Only Dual Stick mode is supported now

    // Dual stick mode - each joystick controls one motor
    int16_t leftJoystickY = controller->axisY();
    int16_t rightJoystickY = controller->axisRY();

    // Apply dead zone
    if (abs(leftJoystickY) < JOYSTICK_DEAD_ZONE)
        leftJoystickY = 0;
    if (abs(rightJoystickY) < JOYSTICK_DEAD_ZONE)
        rightJoystickY = 0;

    // Constrain joystick values
    leftJoystickY = constrain(leftJoystickY, -512, 512);
    rightJoystickY = constrain(rightJoystickY, -512, 512);

    // Map joystick values to motor power
    int leftMotorPower = map(abs(leftJoystickY), 0, 512, 0, 255);
    int rightMotorPower = map(abs(rightJoystickY), 0, 512, 0, 255);

    // Apply motor direction based on joystick position
    if (leftJoystickY > 0)
        motors.leftForward(leftMotorPower);
    else if (leftJoystickY < 0)
        motors.leftBackward(leftMotorPower);
    else
        motors.leftStop();

    if (rightJoystickY > 0)
        motors.rightForward(rightMotorPower);
    else if (rightJoystickY < 0)
        motors.rightBackward(rightMotorPower);
    else
        motors.rightStop();
}

/**
 * Handle calibration buttons
 */
void handleCalibrationButtons(ControllerPtr controller)
{
    if (millis() - lastButtonPressTime <= DEBOUNCE_DELAY)
        return;

    bool calibrationChanged = false;

    // A button - Decrease right motor calibration
    if (controller->a())
    {
        float newCalibration = constrain(motors.getRightCalibration() - CALIBRATION_STEP, 0.0, 1.0);
        motors.setRightCalibration(newCalibration);
        preferences.putFloat("rightCal", newCalibration);
        calibrationChanged = true;
    }

    // Y button - Increase right motor calibration
    if (controller->y())
    {
        float newCalibration = constrain(motors.getRightCalibration() + CALIBRATION_STEP, 0.0, 1.0);
        motors.setRightCalibration(newCalibration);
        preferences.putFloat("rightCal", newCalibration);
        calibrationChanged = true;
    }

    // D-pad UP - Increase left motor calibration
    if (controller->dpad() == DPAD_UP)
    {
        float newCalibration = constrain(motors.getLeftCalibration() + CALIBRATION_STEP, 0.0, 1.0);
        motors.setLeftCalibration(newCalibration);
        preferences.putFloat("leftCal", newCalibration);
        calibrationChanged = true;
    }

    // D-pad DOWN - Decrease left motor calibration
    if (controller->dpad() == DPAD_DOWN)
    {
        float newCalibration = constrain(motors.getLeftCalibration() - CALIBRATION_STEP, 0.0, 1.0);
        motors.setLeftCalibration(newCalibration);
        preferences.putFloat("leftCal", newCalibration);
        calibrationChanged = true;
    }

    if (calibrationChanged)
        lastButtonPressTime = millis();
}

/**
 * This function processes the connected controller
 */
void processController()
{
    if (connectedController && connectedController->isConnected() && connectedController->hasData())
    {
        if (connectedController->isGamepad())
        {
            handleMovement(connectedController);
            handleCalibrationButtons(connectedController);
        }
        else
        {
            Serial.println("Unsupported controller type");
        }
    }
}

/**
 * This function runs once when the Arduino starts
 */
void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nTank Robot Controller Starting...");

    // Print firmware information
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t *addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n",
                  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Set up the Bluepad32 library
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // Forget all previously paired Bluetooth devices (optional)
    BP32.forgetBluetoothKeys();

    // Disable virtual devices (e.g., mouse emulation for gamepads)
    BP32.enableVirtualDevice(false);

    // Initialize preferences for persistent storage
    preferences.begin("robot", false);

    // Load saved calibration values or use defaults
    float leftCal = constrain(preferences.getFloat("leftCal", DEFAULT_LEFT_CALIBRATION), 0.0, 1.0);
    float rightCal = constrain(preferences.getFloat("rightCal", DEFAULT_RIGHT_CALIBRATION), 0.0, 1.0);

    // Initialize the motors with saved calibration values
    motors.begin();
    motors.setLeftCalibration(leftCal);
    motors.setRightCalibration(rightCal);

    Serial.println("Setup complete. Waiting for controller connection...");
}

/**
 * This function runs repeatedly in a loop
 */
void loop()
{
    // Update Bluepad32 and process controller
    bool dataUpdated = BP32.update();
    if (dataUpdated)
    {
        processController();
    }

    // Safety check - if no controller updates for 3 seconds, stop motors
    static unsigned long lastUpdateTime = 0;
    if (dataUpdated)
    {
        lastUpdateTime = millis();
    }
    else if (connectedController != nullptr && millis() - lastUpdateTime > 3000)
    {
        Serial.println("WARNING: No controller updates for 3 seconds, stopping motors");
        motors.stop();
    }
}