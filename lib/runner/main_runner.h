#ifndef MAIN_RUNNER_H
#define MAIN_RUNNER_H

#include "runner.h"
#include "joystick.h"
#include "motor.h"
#include "control_mapper.h"
#include "lcd.h"
#include <Wire.h>

class MainRunner : public Runner
{
private:
    // Component instances
    JoystickController joystick;
    MotorController motor;
    SimpleControlMapper mapper;
    LCDController lcdDisplay;

    // Status tracking
    unsigned long lastStatusTime = 0;
    static const unsigned long STATUS_INTERVAL = 2000;

    // Helper methods
    void printSystemStatus(const JoystickPosition &joy, const SimpleMotorCommand &cmd)
    {
        Serial.println("=== STATUS ===");
        Serial.print("Joystick - X: ");
        Serial.print(joy.x);
        Serial.print(" (");
        Serial.print(joy.x > DIRECTION_DEAD_ZONE ? "FORWARD" : joy.x < -DIRECTION_DEAD_ZONE ? "BACKWARD"
                                                                                            : "NEUTRAL");
        Serial.print(") | Y: ");
        Serial.print(joy.y);
        Serial.print(" (Speed: ");
        Serial.print(cmd.speedPercent);
        Serial.println("%)");

        motor.printStatus();
        Serial.println("==============");
    }

    void initializeI2C()
    {
        // Initialize I2C with custom pins
        Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
        Serial.print("I2C initialized - SDA: ");
        Serial.print(LCD_SDA_PIN);
        Serial.print(", SCL: ");
        Serial.println(LCD_SCL_PIN);
    }

public:
    void setup() override
    {
        Serial.begin(SERIAL_BAUD);
        Serial.println("=== SIMPLE JOYSTICK MOTOR CONTROL ===");
        Serial.println("X-axis: Direction (Left/Right → Back/Forward)");
        Serial.println("Y-axis: Speed (Up = Faster)");
        Serial.println("=====================================");

        // Initialize I2C first
        initializeI2C();

        // Initialize LCD
        lcdDisplay.begin();
        lcdDisplay.displayInstruction("System Starting", "Please wait...");

        // Initialize components
        joystick.begin();
        // motor.begin();
        // mapper.begin();

        // Display calibration instruction
        Serial.println("Starting joystick calibration...");

        // Calibrate joystick
        lcdDisplay.displayInstruction("Calibrating", "Keep centered");
        joystick.calibrate_center();

        lcdDisplay.displayInstruction("Calibrating", "Move Joystick");
        joystick.calibrate_range();

        // Display calibration result
        if (joystick.isCalibrated())
        {
            lcdDisplay.displayInstruction("Calibration", "Success!");
        }
        else
        {
            lcdDisplay.displayInstruction("Calibration", "Failed!");
        }

        Serial.println("=== READY FOR CONTROL ===");
        Serial.println("Move joystick:");
        Serial.println("- Left/Right: Choose direction");
        Serial.println("- Up: Increase speed");
        Serial.println("- Center: Stop motor");
        Serial.println("========================");

        // Display ready message
        lcdDisplay.displayTwoLineMessage("System Ready", "Move joystick", 1000);
    }

    void loop() override
    {
        // Update motor (important for ramping)
        // motor.update();

        // Read joystick position
        JoystickPosition joyPos = joystick.read();

        // Process joystick input
        SimpleMotorCommand motorCmd = mapper.processInput(joyPos);

        // Update LCD with real-time data
        lcdDisplay.displayJoystickStatus(joyPos, motorCmd);

        // Execute motor command if it has changed
        if (motorCmd.hasChanged)
        {
            // mapper.printCommand(motorCmd);

            if (motorCmd.direction == MOTOR_STOP || motorCmd.speedPercent == 0)
            {
                // motor.stop();
                Serial.println("Motor stopped");
            }
            else
            {
                // motor.move(motorCmd.direction, motorCmd.speedPWM);
                String direction = (motorCmd.direction == MOTOR_FORWARD) ? "Forward" : "Backward";
                Serial.println("Motor: " + direction + " at " + String(motorCmd.speedPercent) + "%");
            }
        }

        // Print periodic status to Serial
        unsigned long currentTime = millis();
        if (currentTime - lastStatusTime >= STATUS_INTERVAL)
        {
            printSystemStatus(joyPos, motorCmd);
            lastStatusTime = currentTime;
        }

        delay(LOOP_DELAY);
    }
};

#endif