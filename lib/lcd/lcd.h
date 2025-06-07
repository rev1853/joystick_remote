#ifndef LCD_H
#define LCD_H

#include "config.h"
#include "joystick.h"
#include "control_mapper.h"
#include <LiquidCrystal_I2C.h>

class LCDController
{
private:
    LiquidCrystal_I2C lcd;
    unsigned long lastUpdateTime;
    bool isInitialized;

    // Display state tracking
    String lastLine1;
    String lastLine2;

    // Helper methods
    String formatJoystickData(const JoystickPosition &joy, const SimpleMotorCommand &cmd);
    String formatDirectionSpeed(const SimpleMotorCommand &cmd);
    void updateDisplay(const String &line1, const String &line2);
    String getDirectionString(MotorDirection direction);

public:
    LCDController();

    // Core functions
    void begin();
    void clear();
    void backlight(bool on = true);

    // Display functions
    void displayJoystickStatus(const JoystickPosition &joy, const SimpleMotorCommand &cmd);
    void displayMessage(const String &message, int duration = 0);
    void displayTwoLineMessage(const String &line1, const String &line2, int duration = 0);
    void displayInstruction(const String &instruction, const String &result = "");

    // Utility functions
    void update(); // Call in loop for timed updates
    bool isReady() const;
    void printDebug() const;
};

#endif