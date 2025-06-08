#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions for ESP32
// Better alternatives using ADC1:
const int X_PIN = 34; // GPIO34 (ADC1_CH6) - input only, no pullup
const int Y_PIN = 35; // GPIO35 (ADC1_CH7) - input only, no pullup

// ESP32 ADC specifications
const int ADC_RESOLUTION = 12;  // ESP32 has 12-bit ADC
const int ADC_MAX_VALUE = 4095; // 2^12 - 1
const int ADC_MIN_VALUE = 0;
const int ADC_DEFAULT_CENTER = 2048; // Theoretical center for 12-bit

// Calibration settings (adjusted for ESP32)
const int CALIBRATION_SAMPLES = 100; // More samples for better accuracy
const int CALIBRATION_DELAY = 20;
const int CENTER_CALIBRATION_TIME = 3000;
const int RANGE_CALIBRATION_TIME = 8000; // Longer time for better range detection

// Filter settings (adjusted for ESP32 noise characteristics)
const int FILTER_SAMPLES = 5; // More samples for ESP32 ADC noise

// Joystick settings (adjusted for 12-bit range with extended resolution)
const int DEAD_ZONE_PERCENT = 40; // Adjusted for new range (8% of 500 = 40)
const int MIN_OUTPUT = -500;
const int MAX_OUTPUT = 500;

// ESP32 ADC attenuation settings (affects voltage range)
// ADC_ATTEN_DB_0:   ~800mV range  (most sensitive)
// ADC_ATTEN_DB_2_5: ~1100mV range
// ADC_ATTEN_DB_6:   ~1350mV range
// ADC_ATTEN_DB_11:  ~2600mV range (least sensitive, most common for 3.3V)
const int ADC_ATTENUATION = 3; // ADC_ATTEN_DB_11 = 3

// Serial settings
const int SERIAL_BAUD = 115200; // ESP32 typically uses higher baud rate
const int LOOP_DELAY = 50;      // Faster loop for ESP32

// Motor pins (ESP32 has different PWM characteristics)
const int MOTOR_ENA_PIN = 3; // Make sure this supports PWM on ESP32
const int MOTOR_IN1_PIN = 4;
const int MOTOR_IN2_PIN = 5;

// Motor settings (ESP32 PWM is different from Arduino)
const int PWM_RESOLUTION = 8;   // ESP32 PWM resolution (8-bit = 0-255)
const int PWM_FREQUENCY = 5000; // ESP32 PWM frequency in Hz
const int PWM_CHANNEL = 0;      // ESP32 PWM channel for motor ENA pin

const int MIN_SPEED = 0;
const int MAX_SPEED = 255; // 2^8 - 1 for 8-bit PWM
const int DEFAULT_SPEED = 150;
const int STOP_SPEED = 0;

// Speed ramping settings
const int RAMP_DELAY = 20;
const int RAMP_STEP = 5;

// Control settings (adjusted for 12-bit precision and extended range)
const int DIRECTION_DEAD_ZONE = 40; // Adjusted for new range (8% of 500 = 40)
const int SPEED_DEAD_ZONE = 40;     // Adjusted for new range (8% of 500 = 40)
const int MIN_MOTOR_SPEED = 125;    // Minimum useful motor speed (25% of 500 = 125)

// LCD I2C settings (ESP32 has flexible I2C pins)
const int LCD_ADDRESS = 0x27;
const int LCD_SDA_PIN = 18; // ESP32 I2C SDA
const int LCD_SCL_PIN = 19; // ESP32 I2C SCL
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// LCD timing settings
const int LCD_UPDATE_INTERVAL = 150; // Faster updates for ESP32
const int LCD_STARTUP_DELAY = 2000;
const int LCD_INSTRUCTION_DELAY = 1500;

// ESP32 specific timing
const int ESP32_ADC_STABILIZATION_DELAY = 1; // Small delay for ADC stabilization

// Motor directions
enum MotorDirection
{
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1,
    MOTOR_BACKWARD = 2
};

// ESP32 ADC attenuation enum (for reference)
enum ADC_Attenuation
{
    ADC_ATTEN_DB_0 = 0,   // ~800mV
    ADC_ATTEN_DB_2_5 = 1, // ~1100mV
    ADC_ATTEN_DB_6 = 2,   // ~1350mV
    ADC_ATTEN_DB_11 = 3   // ~2600mV
};

#endif