#ifndef MOTOR
#define MOTOR

#include <stdbool.h>

typedef bool motor_direction;

#define DIRECTION_FORWARD false
#define DIRECTION_BACKWARD true

typedef struct motor_options {
    int pwmFrequency;
    int minPowerInput;
    int maxPowerInput;
    int minPowerValue;
    int inertiaTreshold;
    int forwardSignalPin;
    int backwardSignalPin;
} motor_options;

typedef struct motor {
    bool started;
    bool starting;
    int prevPower;
    int currentPower;
    motor_direction direction;
    motor_options *options;
} motor;

motor *motor_init(motor_options *options);

void motor_set_power(motor* incoming, int power);

void motor_set_direction(motor *incoming, motor_direction direction);

#endif