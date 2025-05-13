#ifndef NON_BLOCKING_DELAY_H
#define NON_BLOCKING_DELAY_H

#include <Arduino.h>

/**
 * NON-BLOCKING DELAY
 * 
 * This is like a timer that lets your robot do multiple things at once!
 * 
 * Normal delays (using delay()) make your robot freeze and wait, which means
 * it can't do anything else during that time. This special delay lets your
 * robot keep doing other things while it waits for the timer to finish.
 */
class NonBlockingDelay {
public:
    /**
     * Create a new timer
     * 
     * @param delayTime How long to wait (in milliseconds)
     */
    NonBlockingDelay(unsigned long delayTime = 0);
    
    /**
     * Start the timer
     * 
     * @param delayTime How long to wait (in milliseconds)
     */
    void start(unsigned long delayTime = 0);
    
    /**
     * Check if the timer has finished
     * 
     * @return true if the time is up, false if still waiting
     */
    bool elapsed();
    
    /**
     * Restart the timer from the beginning
     */
    void reset();
    
    /**
     * Check how much time is left
     * 
     * @return Time remaining in milliseconds
     */
    unsigned long remaining();
    
    /**
     * Check how much time has passed since the timer started
     * 
     * @return Time elapsed in milliseconds
     */
    unsigned long elapsedTime();
    
private:
    unsigned long _startTime;   // When the timer was started
    unsigned long _delayTime;   // How long to wait
    bool _running;              // Is the timer currently running?
};

#endif // NON_BLOCKING_DELAY_H