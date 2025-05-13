#ifndef CONTROLLER_HANDLER_H
#define CONTROLLER_HANDLER_H

#include <Bluepad32.h>
#include <Preferences.h>
#include "MotionMotors.h"
#include "Logger.h"

/**
 * CONTROLLER HANDLER
 * 
 * This handles all the buttons and joysticks on your game controller!
 * It translates button presses and joystick movements into actions
 * that make your robot move.
 */

// Button debounce settings - prevents multiple actions from a single press
#define DEBOUNCE_DELAY 300 // Milliseconds to wait before accepting another button press

// Joystick settings
#define JOYSTICK_DEAD_ZONE 5000 // Ignore joystick values within this range (prevents drift)

// Calibration step size
#define CALIBRATION_STEP 0.05f // How much to change calibration with each button press

class ControllerHandler {
public:
    /**
     * Initialize the controller handler
     * 
     * This gets everything ready to process controller input
     * 
     * @param motors Reference to the motors that will be controlled
     * @param prefs Reference to preferences for saving settings
     */
    static void begin(MotionMotors& motors, Preferences& prefs);
    
    /**
     * Process controller input
     * 
     * This reads the controller and makes the robot respond
     * 
     * @param controller The game controller to read
     */
    static void processController(ControllerPtr controller);
    
private:
    /**
     * Handle movement controls (joysticks)
     * 
     * This makes the robot drive based on joystick positions
     */
    static void handleMovement(ControllerPtr controller);
    
    /**
     * Handle calibration buttons
     * 
     * This adjusts motor power balance when buttons are pressed
     */
    static void handleCalibrationButtons(ControllerPtr controller);
    
    /**
     * Handle debug buttons
     * 
     * This changes what information the robot shows when buttons are pressed
     */
    static void handleDebugButtons(ControllerPtr controller);
    
    // Reference to motors and preferences
    static MotionMotors* _motors;
    static Preferences* _prefs;
    
    // Button debounce
    static unsigned long _lastButtonPressTime;
    
    /**
     * Check if a button can be processed
     * 
     * This prevents multiple actions from a single button press
     * 
     * @return true if enough time has passed since the last button press
     */
    static bool canProcessButton();
};

#endif // CONTROLLER_HANDLER_H