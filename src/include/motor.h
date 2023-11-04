#ifndef MOTOR
#define MOTOR

#include <stdbool.h>

typedef bool motor_direction;

#define DIRECTION_FORWARD true
#define DIRECTION_BACKWARD false

typedef struct motor {
    bool started;
    bool starting;
    int prevPower;
    int currentPower;
    motor_direction direction;
} motor;

motor *motor_init();

void motor_set_power(motor* incoming, int power);

#endif