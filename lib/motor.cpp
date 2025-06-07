#include "motor.h"

MotorController::MotorController()
{
    currentSpeed = 0;
    currentDirection = MOTOR_STOP;
    lastRampTime = 0;
    targetSpeed = 0;
    isRamping = false;
}

void MotorController::begin()
{
    // Initialize motor pins
    pinMode(MOTOR_ENA_PIN, OUTPUT);
    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);

    // Start with motor stopped
    stop();

    Serial.begin(SERIAL_BAUD);
    Serial.println("Motor Controller initialized");
}

void MotorController::setMotorPins(MotorDirection direction)
{
    switch (direction)
    {
    case MOTOR_FORWARD:
        digitalWrite(MOTOR_IN1_PIN, HIGH);
        digitalWrite(MOTOR_IN2_PIN, LOW);
        break;

    case MOTOR_BACKWARD:
        digitalWrite(MOTOR_IN1_PIN, LOW);
        digitalWrite(MOTOR_IN2_PIN, HIGH);
        break;

    case MOTOR_STOP:
    default:
        digitalWrite(MOTOR_IN1_PIN, LOW);
        digitalWrite(MOTOR_IN2_PIN, LOW);
        break;
    }

    currentDirection = direction;
}

void MotorController::setSpeed(int speed)
{
    // Constrain speed to valid range
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);

    analogWrite(MOTOR_ENA_PIN, speed);
    currentSpeed = speed;

    // If speed is 0, set direction to stop
    if (speed == 0)
    {
        setMotorPins(MOTOR_STOP);
    }
}

void MotorController::setDirection(MotorDirection direction)
{
    setMotorPins(direction);
}

void MotorController::move(MotorDirection direction, int speed)
{
    setDirection(direction);
    setSpeed(speed);

    Serial.print("Motor: ");
    Serial.print(direction == MOTOR_FORWARD ? "Forward" : direction == MOTOR_BACKWARD ? "Backward"
                                                                                      : "Stop");
    Serial.print(" at speed ");
    Serial.println(speed);
}

void MotorController::stop()
{
    setSpeed(STOP_SPEED);
    setDirection(MOTOR_STOP);
    isRamping = false;
    targetSpeed = 0;
}

void MotorController::emergencyStop()
{
    // Immediate stop without ramping
    analogWrite(MOTOR_ENA_PIN, 0);
    digitalWrite(MOTOR_IN1_PIN, LOW);
    digitalWrite(MOTOR_IN2_PIN, LOW);

    currentSpeed = 0;
    currentDirection = MOTOR_STOP;
    isRamping = false;
    targetSpeed = 0;

    Serial.println("EMERGENCY STOP!");
}

void MotorController::rampToSpeed(int newTargetSpeed)
{
    newTargetSpeed = constrain(newTargetSpeed, MIN_SPEED, MAX_SPEED);
    targetSpeed = newTargetSpeed;
    isRamping = true;
    lastRampTime = millis();
}

void MotorController::moveWithRamp(MotorDirection direction, int speed)
{
    setDirection(direction);
    rampToSpeed(speed);
}

void MotorController::updateRamping()
{
    if (!isRamping)
        return;

    unsigned long currentTime = millis();
    if (currentTime - lastRampTime >= RAMP_DELAY)
    {

        if (currentSpeed < targetSpeed)
        {
            currentSpeed += RAMP_STEP;
            if (currentSpeed >= targetSpeed)
            {
                currentSpeed = targetSpeed;
                isRamping = false;
            }
        }
        else if (currentSpeed > targetSpeed)
        {
            currentSpeed -= RAMP_STEP;
            if (currentSpeed <= targetSpeed)
            {
                currentSpeed = targetSpeed;
                isRamping = false;
            }
        }
        else
        {
            isRamping = false;
        }

        analogWrite(MOTOR_ENA_PIN, currentSpeed);
        lastRampTime = currentTime;
    }
}

void MotorController::update()
{
    updateRamping();
}

int MotorController::getCurrentSpeed() const
{
    return currentSpeed;
}

MotorDirection MotorController::getCurrentDirection() const
{
    return currentDirection;
}

bool MotorController::isMoving() const
{
    return currentSpeed > 0 && currentDirection != MOTOR_STOP;
}

bool MotorController::isRampingActive() const
{
    return isRamping;
}

void MotorController::printStatus() const
{
    Serial.print("Motor Status - Direction: ");
    Serial.print(currentDirection == MOTOR_FORWARD ? "Forward" : currentDirection == MOTOR_BACKWARD ? "Backward"
                                                                                                    : "Stopped");
    Serial.print(", Speed: ");
    Serial.print(currentSpeed);
    Serial.print(" (");
    Serial.print(speedToPercent(currentSpeed));
    Serial.print("%)");
    if (isRamping)
    {
        Serial.print(", Ramping to: ");
        Serial.print(targetSpeed);
    }
    Serial.println();
}

int MotorController::percentToSpeed(int percent)
{
    percent = constrain(percent, 0, 100);
    return map(percent, 0, 100, MIN_SPEED, MAX_SPEED);
}

int MotorController::speedToPercent(int speed)
{
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    return map(speed, MIN_SPEED, MAX_SPEED, 0, 100);
}