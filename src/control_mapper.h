#ifndef CONTROL_MAPPER_H
#define CONTROL_MAPPER_H

#include "config.h"
#include "joystick.h"

struct SimpleMotorCommand
{
    MotorDirection direction;
    int speedPercent; // 0-100%
    int speedPWM;     // 0-255 PWM value
    bool hasChanged;
};

class SimpleControlMapper
{
private:
    SimpleMotorCommand lastCommand;

    MotorDirection determineDirection(int xValue);
    int calculateSpeed(int yValue);
    int percentToPWM(int percent);
    bool hasCommandChanged(const SimpleMotorCommand &newCmd);

public:
    SimpleControlMapper();

    void begin();
    SimpleMotorCommand processInput(const JoystickPosition &joy);
    void printCommand(const SimpleMotorCommand &cmd) const;
};

#endif