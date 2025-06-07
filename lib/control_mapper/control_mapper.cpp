#include "control_mapper.h"
#include <Arduino.h>

SimpleControlMapper::SimpleControlMapper()
{
    lastCommand = {MOTOR_STOP, 0, 0, false};
}

void SimpleControlMapper::begin()
{
    Serial.println("Simple Control Mapper initialized");
    Serial.println("X-axis = Direction | Y-axis = Speed");
}

SimpleMotorCommand SimpleControlMapper::processInput(const JoystickPosition &joy)
{
    SimpleMotorCommand command;

    // Determine direction from X-axis
    command.direction = determineDirection(joy.x);

    // Calculate speed from Y-axis (positive values only)
    command.speedPercent = calculateSpeed(joy.y);
    command.speedPWM = percentToPWM(command.speedPercent);

    // Check if command has changed
    command.hasChanged = hasCommandChanged(command);

    // Update last command
    if (command.hasChanged)
    {
        lastCommand = command;
    }

    return command;
}

MotorDirection SimpleControlMapper::determineDirection(int xValue)
{
    if (abs(xValue) < DIRECTION_DEAD_ZONE)
    {
        return MOTOR_STOP; // In dead zone, no direction
    }
    else if (xValue > 0)
    {
        return MOTOR_FORWARD; // Positive X = Forward
    }
    else
    {
        return MOTOR_BACKWARD; // Negative X = Backward
    }
}

int SimpleControlMapper::calculateSpeed(int yValue)
{
    // Only use positive Y values for speed
    if (yValue < SPEED_DEAD_ZONE)
    {
        return 0; // Below dead zone = no speed
    }

    // Map Y value (dead zone to 100) to speed (min to 100%)
    int speed = map(yValue, SPEED_DEAD_ZONE, 100, MIN_MOTOR_SPEED, 100);
    return constrain(speed, 0, 100);
}

int SimpleControlMapper::percentToPWM(int percent)
{
    return map(percent, 0, 100, MIN_SPEED, MAX_SPEED);
}

bool SimpleControlMapper::hasCommandChanged(const SimpleMotorCommand &newCmd)
{
    return (newCmd.direction != lastCommand.direction ||
            abs(newCmd.speedPercent - lastCommand.speedPercent) > 5);
}

void SimpleControlMapper::printCommand(const SimpleMotorCommand &cmd) const
{
    if (cmd.hasChanged)
    {
        Serial.print("Command - Direction: ");

        if (cmd.direction == MOTOR_FORWARD)
        {
            Serial.print("FORWARD");
        }
        else if (cmd.direction == MOTOR_BACKWARD)
        {
            Serial.print("BACKWARD");
        }
        else
        {
            Serial.print("STOP");
        }

        Serial.print(" | Speed: ");
        Serial.print(cmd.speedPercent);
        Serial.print("% (PWM: ");
        Serial.print(cmd.speedPWM);
        Serial.println(")");
    }
}