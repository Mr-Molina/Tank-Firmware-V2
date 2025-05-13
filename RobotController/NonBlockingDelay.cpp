#include "NonBlockingDelay.h"

/**
 * Create a new timer
 * 
 * This sets up a timer that can count time without stopping your robot.
 */
NonBlockingDelay::NonBlockingDelay(unsigned long delayTime) {
    _delayTime = delayTime;
    _startTime = 0;
    _running = false;
}

/**
 * Start the timer
 * 
 * This is like pressing start on a stopwatch. It begins counting time.
 */
void NonBlockingDelay::start(unsigned long delayTime) {
    // If a new delay time is provided, use it
    if (delayTime > 0) {
        _delayTime = delayTime;
    }
    
    // Record the current time as the start time
    _startTime = millis();
    _running = true;
}

/**
 * Check if the timer has finished
 * 
 * This is like checking if time is up on a timer.
 * Returns true if the time has passed, false if still waiting.
 */
bool NonBlockingDelay::elapsed() {
    // If the timer isn't running, it's already done
    if (!_running) return true;
    
    // Get the current time
    unsigned long currentTime = millis();
    
    // Check if enough time has passed
    if (currentTime - _startTime >= _delayTime) {
        _running = false;  // Timer is done
        return true;
    }
    
    // Still waiting
    return false;
}

/**
 * Restart the timer from the beginning
 * 
 * This is like resetting a stopwatch to zero and starting it again.
 */
void NonBlockingDelay::reset() {
    _startTime = millis();
    _running = true;
}

/**
 * Check how much time is left
 * 
 * This tells you how many milliseconds are left before the timer finishes.
 */
unsigned long NonBlockingDelay::remaining() {
    // If the timer isn't running, there's no time left
    if (!_running) return 0;
    
    // Get the current time
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - _startTime;
    
    // If more time has passed than the delay time, return 0
    if (elapsedTime >= _delayTime) {
        return 0;
    }
    
    // Calculate the remaining time
    return _delayTime - elapsedTime;
}

/**
 * Check how much time has passed since the timer started
 * 
 * This tells you how many milliseconds have passed since the timer began.
 */
unsigned long NonBlockingDelay::elapsedTime() {
    // If the timer isn't running, return the full delay time
    if (!_running) return _delayTime;
    
    // Calculate how much time has passed
    return millis() - _startTime;
}