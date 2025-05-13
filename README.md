# 🤖 Tank Robot Controller 🎮

Welcome to the Tank Robot Controller project! This is the software that makes your robot tank move and respond to your commands. Think of it as the robot's brain!

## What This Project Does

This project lets you control a robot tank using a game controller (like the ones you use for video games). You can:

- Drive the tank forward and backward
- Turn left and right
- Adjust how fast each wheel spins
- See information about your robot on your computer

## How to Use Your Controller

### Movement Controls
- **Left Joystick (Up/Down)**: Controls the left wheel
- **Right Joystick (Up/Down)**: Controls the right wheel

### Wheel Power Adjustment
Sometimes one wheel might spin faster than the other. You can fix this with these buttons:
- **A Button**: Make the right wheel 5% slower
- **Y Button**: Make the right wheel 5% faster
- **D-pad UP**: Make the left wheel 5% faster
- **D-pad DOWN**: Make the left wheel 5% slower

### Debug Controls
These buttons help you see what's happening inside your robot:
- **L1+R1 (pressed together)**: Change how much information the robot shows
- **L2+R2 (pressed together)**: Turn all debugging on/off
- **X Button**: Show controller information
- **B Button**: Show sensor information

## Pairing Your PS4 Controller

To connect your PS4 controller to the robot:

1. Press and hold the **PS button** and **Share button** at the same time
2. Keep holding until the light on the front of the controller starts to blink
3. Wait for the controller to connect to the robot
4. When the light becomes solid, your controller is connected!

Once a controller is paired, the robot will reject connections from other controllers. This means only your controller can drive the robot.

## How It Works

### The Robot's Brain
The main program (`RobotController.ino`) connects to your controller and tells the motors what to do. It's like the robot's brain that makes all the decisions.

### The Robot's Muscles
The motors are like the robot's muscles. They make the wheels turn so the robot can move:
- `Motor.h` and `Motor.cpp`: Control a single motor
- `MotionMotors.h` and `MotionMotors.cpp`: Control both motors together

### The Robot's Voice
The robot can talk to you through your computer! It sends messages to tell you what it's doing:
- `Logger.h` and `Logger.cpp`: Let the robot send messages
- `Telemetry.h` and `Telemetry.cpp`: Send detailed information about the robot's status

### The Robot's Ears
The robot listens to your controller to know what you want it to do:
- `ControllerHandler.h` and `ControllerHandler.cpp`: Understand controller buttons and joysticks

### The Robot's Patience
Sometimes the robot needs to wait without freezing up:
- `NonBlockingDelay.h` and `NonBlockingDelay.cpp`: Let the robot wait while still doing other things

## Cool Features

### Smooth Acceleration
The robot doesn't just start and stop suddenly - it speeds up and slows down smoothly, just like a real vehicle!

### Motor Calibration
If one wheel is spinning faster than the other, you can adjust them to make your robot drive straight.

### Telemetry Dashboard
The robot sends information to your computer so you can see what's happening, like a dashboard in a car.

### Debug Levels
You can choose how much information the robot shares with you:
- **None**: The robot stays silent
- **Basic**: The robot tells you important things only
- **Detailed**: The robot gives you more details
- **Verbose**: The robot tells you everything it's doing

## How to Get Started

1. Connect your robot to your computer
2. Upload this code to your robot
3. Turn on your game controller and connect it to the robot
4. Start driving!

## Troubleshooting

If your robot isn't working right, try these steps:

1. Make sure your controller is connected (you'll see a message when it connects)
2. Check that your robot's batteries aren't low
3. If the robot is driving in circles, use the calibration buttons to balance the wheels
4. If nothing is happening, try turning on debugging with L1+R1 to see more information

## Learn More

This project uses several important programming concepts:

- **Classes**: Ways to organize code (like Motor, Logger, etc.)
- **Functions**: Small pieces of code that do specific jobs
- **Variables**: Places to store information
- **Loops**: Code that repeats over and over
- **Conditionals**: Making decisions (if this happens, do that)

By exploring this code, you can learn how these concepts work together to make a robot move!

---

Have fun with your robot tank! 🚀