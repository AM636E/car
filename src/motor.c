#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../libs/pigpio.h"

#include "include/gpio.h"
#include "include/motor.h"

motor *motor_init(motor_options *options)
{
    printf("motor init\n");

    gpio_error gpioError;
    gpio_init_pwm(options->pwmFrequency, &gpioError);

    if (gpioError == GPIO_ERROR_INIT)
    {
        printf("gpio init error.");
        exit(1);
    }

    motor *result = (motor *)malloc(sizeof(motor));
    result->currentPower = result->prevPower = 0;
    result->direction = DIRECTION_FORWARD;
    result->started = result->starting = false;
    result->options = options;

    motor_set_direction(result, DIRECTION_FORWARD);

    printf("motor options: F: %i; \n", options->pwmFrequency);

    return result;
}

void motor_set_direction(motor *incoming, motor_direction direction)
{
    switch (direction)
    {
    case DIRECTION_FORWARD:
    {
        gpio_write(incoming->options->forwardSignalPin, HIGH);
        gpio_write(incoming->options->backwardSignalPin, LOW);
        break;
    }
    case DIRECTION_BACKWARD:
    {
        gpio_write(incoming->options->forwardSignalPin, LOW);
        gpio_write(incoming->options->backwardSignalPin, HIGH);
        break;
    }
    }

    incoming->direction = direction;
}

void motor_set_power(motor *incoming, int power)
{
    if (incoming == NULL)
    {
        fprintf(stderr, "invalid motor\n");
        exit(1);
    }

    printf("set power: %i. started: %i\n", power, incoming->started);

    if (power > incoming->options->minPowerInput && power <= incoming->options->minPowerValue)
    {
        power = incoming->options->minPowerValue;
    }

    if (incoming->starting)
    {
        printf("set power skip: starting...\n");
        return;
    }

    if (power < incoming->options->minPowerValue)
    {
        gpio_write_pwm(0);
        incoming->started = false;

        return;
    }

    if (power < incoming->options->inertiaTreshold && !incoming->started)
    {
        incoming->starting = true;
        printf("starting the motor:\n");
        for (int i = incoming->options->inertiaTreshold; i >= power; i -= 50)
        {
            printf("starting the motor: %i ->[%i]\n", i, power);
            gpio_write_pwm(i);

            usleep(10000);
        }

        incoming->starting = false;
    }

    incoming->started = true;

    gpio_write_pwm(power);
}