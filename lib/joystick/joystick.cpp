#include "joystick.h"
#include <Arduino.h>

JoystickController::JoystickController()
{
    // Initialize calibration data
    calibration.xMin = 0;
    calibration.xMax = 1023;
    calibration.xCenter = 512;
    calibration.yMin = 0;
    calibration.yMax = 1023;
    calibration.yCenter = 512;
    calibration.isCalibrated = false;

    // Initialize filter
    filterIndex = 0;
    for (int i = 0; i < FILTER_SAMPLES; i++)
    {
        xHistory[i] = 0;
        yHistory[i] = 0;
    }
}

void JoystickController::begin()
{
    Serial.begin(SERIAL_BAUD);
}

void JoystickController::calibrate_center()
{
    Serial.println("=== JOYSTICK CALIBRATION ===");
    Serial.println("1. Keep joystick centered...");
    delay(CENTER_CALIBRATION_TIME);

    // Find center position
    long xSum = 0, ySum = 0;
    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
        xSum += analogRead(X_PIN);
        ySum += analogRead(Y_PIN);
        delay(CALIBRATION_DELAY);
    }
    calibration.xCenter = xSum / CALIBRATION_SAMPLES;
    calibration.yCenter = ySum / CALIBRATION_SAMPLES;
}

void JoystickController::calibrate_range()
{
    Serial.println("2. Move joystick to ALL extremes...");
    delay(1000);

    // Find min/max values
    calibration.xMin = calibration.xCenter;
    calibration.xMax = calibration.xCenter;
    calibration.yMin = calibration.yCenter;
    calibration.yMax = calibration.yCenter;

    unsigned long startTime = millis();
    while (millis() - startTime < RANGE_CALIBRATION_TIME)
    {
        int x = analogRead(X_PIN);
        int y = analogRead(Y_PIN);

        if (x < calibration.xMin)
            calibration.xMin = x;
        if (x > calibration.xMax)
            calibration.xMax = x;
        if (y < calibration.yMin)
            calibration.yMin = y;
        if (y > calibration.yMax)
            calibration.yMax = y;

        Serial.print(".");
        delay(50);
    }

    calibration.isCalibrated = true;
    Serial.println("\n=== CALIBRATION COMPLETE ===");
    printCalibrationData();
}

JoystickPosition JoystickController::read()
{
    JoystickPosition position = {0, 0};

    if (!calibration.isCalibrated)
    {
        return position;
    }

    // Read raw values
    int xRaw = analogRead(X_PIN);
    int yRaw = analogRead(Y_PIN);

    // Map to output range
    int xMapped = mapToRange(xRaw, calibration.xMin, calibration.xMax, calibration.xCenter);
    int yMapped = mapToRange(yRaw, calibration.yMin, calibration.yMax, calibration.yCenter);

    // Apply smoothing
    int xSmooth = applySmoothing(xMapped, xHistory);
    int ySmooth = applySmoothing(yMapped, yHistory);

    // Update filter index
    filterIndex = (filterIndex + 1) % FILTER_SAMPLES;

    // Apply dead zone
    position.x = (abs(xSmooth) < DEAD_ZONE_PERCENT) ? 0 : xSmooth;
    position.y = (abs(ySmooth) < DEAD_ZONE_PERCENT) ? 0 : ySmooth;

    return position;
}

int JoystickController::applySmoothing(int newValue, int *history)
{
    history[filterIndex] = newValue;

    int sum = 0;
    for (int i = 0; i < FILTER_SAMPLES; i++)
    {
        sum += history[i];
    }

    return sum / FILTER_SAMPLES;
}

int JoystickController::mapToRange(int rawValue, int minVal, int maxVal, int centerVal)
{
    int mapped;

    if (rawValue >= centerVal)
    {
        mapped = map(rawValue, centerVal, maxVal, 0, MAX_OUTPUT);
    }
    else
    {
        mapped = map(rawValue, minVal, centerVal, MIN_OUTPUT, 0);
    }

    return constrain(mapped, MIN_OUTPUT, MAX_OUTPUT);
}

bool JoystickController::isCalibrated() const
{
    return calibration.isCalibrated;
}

void JoystickController::printCalibrationData() const
{
    Serial.println("X - Min: " + String(calibration.xMin) +
                   " Center: " + String(calibration.xCenter) +
                   " Max: " + String(calibration.xMax));
    Serial.println("Y - Min: " + String(calibration.yMin) +
                   " Center: " + String(calibration.yCenter) +
                   " Max: " + String(calibration.yMax));
    Serial.println("=============================\n");
}
