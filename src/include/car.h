#ifndef STEERING
#define STEERING

#include "motor.h"
#include "gpio.h"

#define STEERING_PIN 17

typedef struct car
{
    motor *carMotor;
    value_range *steeringServoRange;
    int steeringPosition;
} car;


car *car_init(motor_options *options, value_range *steeringRange);

void car_turn(car *incoming, int incomingValue);

void car_turn_raw(car *incoming, int incomingValue);
#endif