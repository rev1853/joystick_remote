#include "lcd.h"
#include <Arduino.h>

LCDController::LCDController() : lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS)
{
    lastUpdateTime = 0;
    isInitialized = false;
    lastLine1 = "";
    lastLine2 = "";
}

void LCDController::begin()
{
    // Initialize I2C LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Display startup message
    lcd.setCursor(0, 0);
    lcd.print("Joystick Control");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");

    delay(LCD_STARTUP_DELAY);

    isInitialized = true;
    lastUpdateTime = millis();

    Serial.println("LCD Controller initialized");
    Serial.print("LCD Address: 0x");
    Serial.println(LCD_ADDRESS, HEX);
    Serial.print("LCD Size: ");
    Serial.print(LCD_COLS);
    Serial.print("x");
    Serial.println(LCD_ROWS);
}

void LCDController::clear()
{
    if (!isInitialized)
        return;

    lcd.clear();
    lastLine1 = "";
    lastLine2 = "";
}

void LCDController::backlight(bool on)
{
    if (!isInitialized)
        return;

    if (on)
    {
        lcd.backlight();
    }
    else
    {
        lcd.noBacklight();
    }
}

void LCDController::displayJoystickStatus(const JoystickPosition &joy, const SimpleMotorCommand &cmd)
{
    if (!isInitialized)
        return;

    // Only update if enough time has passed (reduce flickering)
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime < LCD_UPDATE_INTERVAL)
    {
        return;
    }

    String line1 = formatJoystickData(joy, cmd);
    String line2 = formatDirectionSpeed(cmd);

    updateDisplay(line1, line2);
    lastUpdateTime = currentTime;
}

String LCDController::formatJoystickData(const JoystickPosition &joy, const SimpleMotorCommand &cmd)
{
    // Format: "45,-23 67%"
    String line = String(joy.x) + "," + String(joy.y) + " " + String(cmd.speedPercent) + "%";

    // Pad with spaces to fill the line and clear any previous text
    while (line.length() < LCD_COLS)
    {
        line += " ";
    }

    return line;
}

String LCDController::formatDirectionSpeed(const SimpleMotorCommand &cmd)
{
    // Format: "BACKWARD" (just the direction, no "DIR:" prefix)
    String line = getDirectionString(cmd.direction);

    // Pad the rest with spaces to clear any previous text
    while (line.length() < LCD_COLS)
    {
        line += " ";
    }

    return line;
}

String LCDController::getDirectionString(MotorDirection direction)
{
    switch (direction)
    {
    case MOTOR_FORWARD:
        return "FORWARD";
    case MOTOR_BACKWARD:
        return "BACKWARD";
    case MOTOR_STOP:
    default:
        return "STOPPED";
    }
}

void LCDController::updateDisplay(const String &line1, const String &line2)
{
    // Only update if content has changed
    if (line1 != lastLine1)
    {
        lcd.setCursor(0, 0);
        lcd.print(line1);
        lastLine1 = line1;
    }

    if (line2 != lastLine2)
    {
        lcd.setCursor(0, 1);
        lcd.print(line2);
        lastLine2 = line2;
    }
}

void LCDController::displayMessage(const String &message, int duration)
{
    if (!isInitialized)
        return;

    clear();
    lcd.setCursor(0, 0);

    // Handle message longer than LCD width
    if (message.length() <= LCD_COLS)
    {
        lcd.print(message);
    }
    else
    {
        lcd.print(message.substring(0, LCD_COLS));
        lcd.setCursor(0, 1);
        lcd.print(message.substring(LCD_COLS, LCD_COLS * 2));
    }

    if (duration > 0)
    {
        delay(duration);
    }
}

void LCDController::displayTwoLineMessage(const String &line1, const String &line2, int duration)
{
    if (!isInitialized)
        return;

    clear();

    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, LCD_COLS));

    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, LCD_COLS));

    lastLine1 = line1;
    lastLine2 = line2;

    if (duration > 0)
    {
        delay(duration);
    }
}

void LCDController::displayInstruction(const String &instruction, const String &result)
{
    if (!isInitialized)
        return;

    Serial.println("LCD Instruction: " + instruction);
    if (result.length() > 0)
    {
        Serial.println("LCD Result: " + result);
    }

    // Display instruction
    displayMessage(instruction, LCD_INSTRUCTION_DELAY);

    // Display result if provided
    if (result.length() > 0)
    {
        displayMessage("Result: " + result, LCD_INSTRUCTION_DELAY);
    }
}

void LCDController::update()
{
    // This method can be used for future enhancements like scrolling text
    // Currently, the main updates happen in displayJoystickStatus
}

bool LCDController::isReady() const
{
    return isInitialized;
}

void LCDController::printDebug() const
{
    Serial.println("=== LCD DEBUG INFO ===");
    Serial.print("Initialized: ");
    Serial.println(isInitialized ? "Yes" : "No");
    Serial.print("Address: 0x");
    Serial.println(LCD_ADDRESS, HEX);
    Serial.print("Size: ");
    Serial.print(LCD_COLS);
    Serial.print("x");
    Serial.println(LCD_ROWS);
    Serial.print("Last Line 1: ");
    Serial.println(lastLine1);
    Serial.print("Last Line 2: ");
    Serial.println(lastLine2);
    Serial.println("=====================");
}