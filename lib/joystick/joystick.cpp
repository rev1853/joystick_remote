#include "joystick.h"
#include <Arduino.h>

JoystickController::JoystickController()
{
    // Initialize calibration data with ESP32 12-bit defaults
    calibration.xMin = ADC_MIN_VALUE;
    calibration.xMax = ADC_MAX_VALUE;
    calibration.xCenter = ADC_DEFAULT_CENTER;
    calibration.yMin = ADC_MIN_VALUE;
    calibration.yMax = ADC_MAX_VALUE;
    calibration.yCenter = ADC_DEFAULT_CENTER;
    calibration.isCalibrated = false;

    // Initialize filter
    filterIndex = 0;
    filterInitialized = false;
    initializeFilter();
}

void JoystickController::initializeFilter()
{
    for (int i = 0; i < FILTER_SAMPLES; i++)
    {
        xHistory[i] = 0;
        yHistory[i] = 0;
    }
    filterInitialized = false;
}

void JoystickController::begin()
{
    Serial.begin(SERIAL_BAUD);

    // Configure ESP32 ADC
    analogReadResolution(ADC_RESOLUTION); // Set to 12-bit resolution

    // Set ADC attenuation for both channels
    analogSetAttenuation((adc_attenuation_t)ADC_ATTENUATION);

    // ESP32-specific: Set pin-specific attenuation if needed
    analogSetPinAttenuation(X_PIN, (adc_attenuation_t)ADC_ATTENUATION);
    analogSetPinAttenuation(Y_PIN, (adc_attenuation_t)ADC_ATTENUATION);

    Serial.println("=== ESP32 Joystick Controller ===");
    Serial.print("ADC Resolution: ");
    Serial.print(ADC_RESOLUTION);
    Serial.print("-bit (0-");
    Serial.print(ADC_MAX_VALUE);
    Serial.println(")");
    Serial.print("ADC Attenuation: ");
    Serial.println(ADC_ATTENUATION);
    Serial.print("X Pin: GPIO");
    Serial.println(X_PIN);
    Serial.print("Y Pin: GPIO");
    Serial.println(Y_PIN);
    Serial.println("================================");

    // Perform initial ADC readings to stabilize
    for (int i = 0; i < 10; i++)
    {
        analogRead(X_PIN);
        analogRead(Y_PIN);
        delay(10);
    }
}

void JoystickController::calibrate_center()
{
    Serial.println("=== JOYSTICK CALIBRATION ===");
    Serial.println("1. Keep joystick centered...");
    Serial.println("Starting in 3 seconds...");
    delay(3000);

    // Find center position with better error handling
    long xSum = 0, ySum = 0;
    int validSamples = 0;
    int xMin = ADC_MAX_VALUE, xMax = ADC_MIN_VALUE;
    int yMin = ADC_MAX_VALUE, yMax = ADC_MIN_VALUE;

    Serial.println("Reading center position...");
    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
        // Small delay for ADC stabilization
        delay(ESP32_ADC_STABILIZATION_DELAY);

        int x = analogRead(X_PIN);
        delay(ESP32_ADC_STABILIZATION_DELAY);
        int y = analogRead(Y_PIN);

        // Validate readings (ESP32 ADC range)
        if (x >= ADC_MIN_VALUE && x <= ADC_MAX_VALUE &&
            y >= ADC_MIN_VALUE && y <= ADC_MAX_VALUE)
        {
            xSum += x;
            ySum += y;
            validSamples++;

            // Track variation during center calibration
            if (x < xMin)
                xMin = x;
            if (x > xMax)
                xMax = x;
            if (y < yMin)
                yMin = y;
            if (y > yMax)
                yMax = y;
        }

        // Progress indicator
        if (i % 20 == 0)
        {
            Serial.print("Progress: ");
            Serial.print((i * 100) / CALIBRATION_SAMPLES);
            Serial.print("% - Current: X=");
            Serial.print(x);
            Serial.print(" Y=");
            Serial.println(y);
        }

        delay(CALIBRATION_DELAY);
    }

    if (validSamples > CALIBRATION_SAMPLES * 0.8) // Need at least 80% valid samples
    {
        calibration.xCenter = xSum / validSamples;
        calibration.yCenter = ySum / validSamples;

        Serial.println("Center calibration successful!");
        Serial.print("X Center: ");
        Serial.print(calibration.xCenter);
        Serial.print(" (variation: ");
        Serial.print(xMax - xMin);
        Serial.println(")");
        Serial.print("Y Center: ");
        Serial.print(calibration.yCenter);
        Serial.print(" (variation: ");
        Serial.print(yMax - yMin);
        Serial.println(")");

        // Check if joystick is too noisy
        if ((xMax - xMin) > 200 || (yMax - yMin) > 200)
        {
            Serial.println("WARNING: High noise detected during center calibration!");
            Serial.println("Consider using a more stable power supply or better connections.");
        }
    }
    else
    {
        Serial.println("ERROR: Center calibration failed - too few valid samples!");
        Serial.print("Valid samples: ");
        Serial.print(validSamples);
        Serial.print(" out of ");
        Serial.println(CALIBRATION_SAMPLES);

        // Use default center values
        calibration.xCenter = ADC_DEFAULT_CENTER;
        calibration.yCenter = ADC_DEFAULT_CENTER;
        Serial.println("Using default center values.");
    }
}

void JoystickController::calibrate_range()
{
    Serial.println("2. Move joystick to ALL extremes (circles and corners)...");
    Serial.println("Starting in 3 seconds...");
    delay(3000);

    // Initialize min/max values with center values
    calibration.xMin = calibration.xCenter;
    calibration.xMax = calibration.xCenter;
    calibration.yMin = calibration.yCenter;
    calibration.yMax = calibration.yCenter;

    unsigned long startTime = millis();
    int sampleCount = 0;
    int progressStep = RANGE_CALIBRATION_TIME / 10; // 10% increments
    unsigned long nextProgressTime = startTime + progressStep;

    Serial.println("Move joystick to all corners and edges NOW!");

    while (millis() - startTime < RANGE_CALIBRATION_TIME)
    {
        delay(ESP32_ADC_STABILIZATION_DELAY);
        int x = analogRead(X_PIN);
        delay(ESP32_ADC_STABILIZATION_DELAY);
        int y = analogRead(Y_PIN);

        // Validate readings
        if (x >= ADC_MIN_VALUE && x <= ADC_MAX_VALUE &&
            y >= ADC_MIN_VALUE && y <= ADC_MAX_VALUE)
        {
            // Update min/max values
            if (x < calibration.xMin)
                calibration.xMin = x;
            if (x > calibration.xMax)
                calibration.xMax = x;
            if (y < calibration.yMin)
                calibration.yMin = y;
            if (y > calibration.yMax)
                calibration.yMax = y;

            sampleCount++;
        }

        // Progress indicator
        unsigned long currentTime = millis();
        if (currentTime >= nextProgressTime)
        {
            int progress = ((currentTime - startTime) * 100) / RANGE_CALIBRATION_TIME;
            Serial.print("Progress: ");
            Serial.print(progress);
            Serial.print("% - Current ranges X: ");
            Serial.print(calibration.xMin);
            Serial.print("-");
            Serial.print(calibration.xMax);
            Serial.print(" Y: ");
            Serial.print(calibration.yMin);
            Serial.print("-");
            Serial.println(calibration.yMax);
            nextProgressTime += progressStep;
        }

        delay(20); // Reasonable sampling rate
    }

    // Validate calibration ranges
    int xRange = calibration.xMax - calibration.xMin;
    int yRange = calibration.yMax - calibration.yMin;

    Serial.print("Range calibration complete. Samples: ");
    Serial.println(sampleCount);

    // ESP32 should achieve much larger ranges due to 12-bit precision
    int minExpectedRange = ADC_MAX_VALUE / 4; // Expect at least 25% of full range

    if (xRange < minExpectedRange || yRange < minExpectedRange)
    {
        Serial.println("WARNING: Calibration range seems too small!");
        Serial.print("X Range: ");
        Serial.print(xRange);
        Serial.print(" (expected > ");
        Serial.print(minExpectedRange);
        Serial.println(")");
        Serial.print("Y Range: ");
        Serial.print(yRange);
        Serial.print(" (expected > ");
        Serial.print(minExpectedRange);
        Serial.println(")");
        Serial.println("Consider:");
        Serial.println("- Moving joystick to more extreme positions");
        Serial.println("- Checking joystick connections");
        Serial.println("- Verifying power supply stability");
    }
    else
    {
        Serial.println("Range calibration successful!");
    }

    calibration.isCalibrated = true;
    Serial.println("\n=== CALIBRATION COMPLETE ===");
    printCalibrationData();

    // Initialize filter with center values
    initializeFilter();
}

JoystickPosition JoystickController::readRaw()
{
    JoystickPosition position;
    delay(ESP32_ADC_STABILIZATION_DELAY);
    position.x = analogRead(X_PIN);
    delay(ESP32_ADC_STABILIZATION_DELAY);
    position.y = analogRead(Y_PIN);
    return position;
}

JoystickPosition JoystickController::read()
{
    JoystickPosition position = {0, 0};

    if (!calibration.isCalibrated)
    {
        Serial.println("WARNING: Joystick not calibrated!");
        return position;
    }

    // Read raw values with ESP32 stabilization
    delay(ESP32_ADC_STABILIZATION_DELAY);
    int xRaw = analogRead(X_PIN);
    delay(ESP32_ADC_STABILIZATION_DELAY);
    int yRaw = analogRead(Y_PIN);

    // Validate raw readings
    if (xRaw < ADC_MIN_VALUE || xRaw > ADC_MAX_VALUE ||
        yRaw < ADC_MIN_VALUE || yRaw > ADC_MAX_VALUE)
    {
        Serial.print("ERROR: Invalid ADC reading - X: ");
        Serial.print(xRaw);
        Serial.print(" Y: ");
        Serial.println(yRaw);
        return position;
    }

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
    // Store new value
    history[filterIndex] = newValue;

    // If filter not fully initialized, fill with current value
    if (!filterInitialized)
    {
        for (int i = 0; i < FILTER_SAMPLES; i++)
        {
            history[i] = newValue;
        }
        filterInitialized = true;
        return newValue;
    }

    // Calculate average
    long sum = 0;
    for (int i = 0; i < FILTER_SAMPLES; i++)
    {
        sum += history[i];
    }

    return (int)(sum / FILTER_SAMPLES);
}

int JoystickController::mapToRange(int rawValue, int minVal, int maxVal, int centerVal)
{
    // Ensure we have valid ranges
    if (maxVal <= minVal)
    {
        Serial.println("ERROR: Invalid calibration range!");
        return 0;
    }

    // Ensure center is within range
    centerVal = constrain(centerVal, minVal, maxVal);

    int mapped;

    if (rawValue >= centerVal)
    {
        // Map upper half (center to max) to (0 to +100)
        if (maxVal == centerVal)
        {
            mapped = 0; // Avoid division by zero
        }
        else
        {
            mapped = map(rawValue, centerVal, maxVal, 0, MAX_OUTPUT);
        }
    }
    else
    {
        // Map lower half (min to center) to (-100 to 0)
        if (minVal == centerVal)
        {
            mapped = 0; // Avoid division by zero
        }
        else
        {
            mapped = map(rawValue, minVal, centerVal, MIN_OUTPUT, 0);
        }
    }

    // Ensure output is within bounds
    mapped = constrain(mapped, MIN_OUTPUT, MAX_OUTPUT);

    return mapped;
}

bool JoystickController::isCalibrated() const
{
    return calibration.isCalibrated;
}

void JoystickController::printCalibrationData() const
{
    Serial.println("=== ESP32 CALIBRATION DATA ===");
    Serial.print("X - Min: ");
    Serial.print(calibration.xMin);
    Serial.print(" (");
    Serial.print((calibration.xMin * 100) / ADC_MAX_VALUE);
    Serial.print("%) Center: ");
    Serial.print(calibration.xCenter);
    Serial.print(" (");
    Serial.print((calibration.xCenter * 100) / ADC_MAX_VALUE);
    Serial.print("%) Max: ");
    Serial.print(calibration.xMax);
    Serial.print(" (");
    Serial.print((calibration.xMax * 100) / ADC_MAX_VALUE);
    Serial.print("%) Range: ");
    Serial.println(calibration.xMax - calibration.xMin);

    Serial.print("Y - Min: ");
    Serial.print(calibration.yMin);
    Serial.print(" (");
    Serial.print((calibration.yMin * 100) / ADC_MAX_VALUE);
    Serial.print("%) Center: ");
    Serial.print(calibration.yCenter);
    Serial.print(" (");
    Serial.print((calibration.yCenter * 100) / ADC_MAX_VALUE);
    Serial.print("%) Max: ");
    Serial.print(calibration.yMax);
    Serial.print(" (");
    Serial.print((calibration.yMax * 100) / ADC_MAX_VALUE);
    Serial.print("%) Range: ");
    Serial.println(calibration.yMax - calibration.yMin);
    Serial.println("==============================");
}

void JoystickController::printDebugInfo(const JoystickPosition &raw, const JoystickPosition &processed) const
{
    Serial.print("Raw ADC: X=");
    Serial.print(raw.x);
    Serial.print(" (");
    Serial.print((raw.x * 100) / ADC_MAX_VALUE);
    Serial.print("%) Y=");
    Serial.print(raw.y);
    Serial.print(" (");
    Serial.print((raw.y * 100) / ADC_MAX_VALUE);
    Serial.print("%) | Processed: X=");
    Serial.print(processed.x);
    Serial.print(" Y=");
    Serial.println(processed.y);
}