#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
const int X_PIN = 14; // Changed from 0 to A0
const int Y_PIN = 27; // Changed from 1 to A1

// Calibration settings
const int CALIBRATION_SAMPLES = 50;
const int CALIBRATION_DELAY = 20;
const int CENTER_CALIBRATION_TIME = 3000;
const int RANGE_CALIBRATION_TIME = 5000;

// Filter settings
const int FILTER_SAMPLES = 3;

// Joystick settings
const int DEAD_ZONE_PERCENT = 8;
const int MIN_OUTPUT = -100;
const int MAX_OUTPUT = 100;

// Serial settings
const int SERIAL_BAUD = 9600;
const int LOOP_DELAY = 100;

// Motor pins
const int MOTOR_ENA_PIN = 3; // Speed control (PWM pin)
const int MOTOR_IN1_PIN = 4; // Direction pin 1
const int MOTOR_IN2_PIN = 5; // Direction pin 2

// Motor settings
const int MIN_SPEED = 0;       // Minimum speed (0-255)
const int MAX_SPEED = 255;     // Maximum speed (0-255)
const int DEFAULT_SPEED = 150; // Default speed
const int STOP_SPEED = 0;      // Stop speed

// Speed ramping settings
const int RAMP_DELAY = 20; // Delay between speed changes (ms)
const int RAMP_STEP = 5;   // Speed change per step

// Control settings
const int DIRECTION_DEAD_ZONE = 15; // Dead zone for X-axis direction
const int SPEED_DEAD_ZONE = 10;     // Dead zone for Y-axis speed
const int MIN_MOTOR_SPEED = 30;     // Minimum useful motor speed (0-100%)

// LCD I2C settings
const int LCD_ADDRESS = 0x27; // I2C address of LCD
const int LCD_SDA_PIN = 18;   // SDA pin
const int LCD_SCL_PIN = 19;   // SCL pin
const int LCD_COLS = 16;      // LCD columns
const int LCD_ROWS = 2;       // LCD rows

// LCD timing settings
const int LCD_UPDATE_INTERVAL = 200;    // Update interval in ms (reduce flickering)
const int LCD_STARTUP_DELAY = 2000;     // Startup message delay
const int LCD_INSTRUCTION_DELAY = 1500; // Instruction display duration

// Motor directions
enum MotorDirection
{
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1,
    MOTOR_BACKWARD = 2
};

#endif