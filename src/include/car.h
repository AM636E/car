#ifndef STEERING
#define STEERING

#include "motor.h"

#define STEERING_PIN 17

typedef struct car
{
    motor *carMotor;
    int steeringPosition;
} car;


car *car_init(motor_options *options);

void car_turn(car *incoming, int incomingValue);

void car_turn_raw(car *incoming, int incomingValue);
#endif