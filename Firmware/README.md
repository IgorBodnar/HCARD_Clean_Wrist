# Firmware
## Overview
This directory contains the firmware files required to operate the CleanWrist prototype.
* **libraries** folder contains custom Arduino library for controlling unipolar stepper motor with a ULN2003 driver.
* **hcard** folder contains an Arduino sketch responsible for sampling the joystick data and using it to control two stepper motors.

## Sketch Structure
### Joystick sampling
The joystick sampling is separated into two stages: potentiometer and button sampling.

Potentiometer samping is wrapped into non-blocking timing wrapper in order to guarantee optimal sampling rate. The sampling itself is done via AnalogRead function.

Button sampling is also wrapped into a millis() based timing wrapper with a shorter sampling period. The period was selected to both allow detection of multiple clicks as well as debouncing the button. The detection loop used DigitalRead to detect falling and rising edges of a button signal. On first falling edge the ClickDetectionHandler is invoked. It then counts the number of rising edges during a pre-determined timeout to distinguish between single/double/triple clicks.

### Motor control
The joystick information obtained in the main loop is cathegorised into three distinct cases:
* Only X axis is non-zero -> motors are controlled to spin in the same direction with equal velocity, which due to mouting of the motors and differential mechanism produces rolling motion.
* Only Y axis is non-zero -> motors are controlled to spin with equal velocity in opposite directions, hence producing rotation about the pitch axis of the device.
* Both X and Y axes are non zero -> directions and velocities of motors are varied to achieve rotation about pitch and roll axes simultaneously.

Unlike the joystick sampling and data interpretation which are performed in the main loop of the sketch, the stepping control of the motors in done as a callback to a timed hardware interrupt. This makes the speed of the motors independent from time required to execute the main loop, providing reliable motor speeds regardless of complexity of the main loop.

It is important to note that the code is not fully decoupled due to the fact that in the microcontroller used (i.e Arduino Nano Every), the hardware timer "Timer1" used to produce interrupts is also used in system time monitoring hence changing the default frequency of said timer affects the millis() and micros() funcitons used throughout the rest of the code. This behaviour is accounted for by selecting the appropriate sampling times for the rest of the timing wrappers.