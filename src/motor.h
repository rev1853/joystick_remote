#ifndef MOTOR_H
#define MOTOR_H

#include "config.h"
#include <Arduino.h>

class MotorController
{
private:
    int currentSpeed;
    MotorDirection currentDirection;
    unsigned long lastRampTime;
    int targetSpeed;
    bool isRamping;

    void setMotorPins(MotorDirection direction);
    void updateRamping();

public:
    MotorController();

    // Basic control
    void begin();
    void setSpeed(int speed);
    void setDirection(MotorDirection direction);
    void move(MotorDirection direction, int speed);
    void stop();
    void emergencyStop();

    // Advanced control
    void rampToSpeed(int targetSpeed);
    void moveWithRamp(MotorDirection direction, int speed);

    // Status functions
    int getCurrentSpeed() const;
    MotorDirection getCurrentDirection() const;
    bool isMoving() const;
    bool isRampingActive() const;

    // Utility functions
    void update(); // Call in loop() for ramping
    void printStatus() const;

    // Speed conversion utilities
    static int percentToSpeed(int percent); // Convert 0-100% to 0-255
    static int speedToPercent(int speed);   // Convert 0-255 to 0-100%
};

#endif