#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "config.h"

// Joystick position structure
struct JoystickPosition
{
    int x;
    int y;
};

struct CalibrationData
{
    int xMin, xMax, xCenter;
    int yMin, yMax, yCenter;
    bool isCalibrated;
};

class JoystickController
{
private:
    CalibrationData calibration;
    int xHistory[FILTER_SAMPLES];
    int yHistory[FILTER_SAMPLES];
    int filterIndex;

    int applySmoothing(int newValue, int *history);
    int mapToRange(int rawValue, int minVal, int maxVal, int centerVal);

public:
    JoystickController();

    void begin();
    void calibrate_center();
    void calibrate_range();
    JoystickPosition read();

    bool isCalibrated() const;
    void printCalibrationData() const;
};

#endif