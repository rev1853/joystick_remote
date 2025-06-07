#ifndef MAIN_RUNNER_H
#define MAIN_RUNNER_H

#include "runner.h"
#include "joystick.h"
#include "motor.h"
#include "control_mapper.h"

class MainRunner : public Runner
{
private:
    // Component instances
    JoystickController joystick;
    MotorController motor;
    SimpleControlMapper mapper;

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

public:
    void setup() override
    {
        Serial.begin(SERIAL_BAUD);
        Serial.println("=== SIMPLE JOYSTICK MOTOR CONTROL ===");
        Serial.println("X-axis: Direction (Left/Right → Back/Forward)");
        Serial.println("Y-axis: Speed (Up = Faster)");
        Serial.println("=====================================");

        // Initialize components
        joystick.begin();
        // motor.begin();
        // mapper.begin();

        // Calibrate joystick
        Serial.println("Starting joystick calibration...");
        joystick.calibrate();

        Serial.println("=== READY FOR CONTROL ===");
        Serial.println("Move joystick:");
        Serial.println("- Left/Right: Choose direction");
        Serial.println("- Up: Increase speed");
        Serial.println("- Center: Stop motor");
        Serial.println("========================");
    }

    void loop() override
    {
        // Update motor (important for ramping)
        // motor.update();

        // Read joystick position
        JoystickPosition joyPos = joystick.read();

        // Process joystick input
        SimpleMotorCommand motorCmd = mapper.processInput(joyPos);

        // Execute motor command if it has changed
        if (motorCmd.hasChanged)
        {
            // mapper.printCommand(motorCmd);

            if (motorCmd.direction == MOTOR_STOP || motorCmd.speedPercent == 0)
            {
                // motor.stop();
            }
            else
            {
                // motor.move(motorCmd.direction, motorCmd.speedPWM);
            }
        }

        // Print periodic status
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