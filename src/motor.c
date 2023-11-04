#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/gpio.h"
#include "include/motor.h"

#define MOTOR_MIN_POWER_INPUT 40
#define MOTOR_MIN_POWER_VALUE 80
#define MOTOR_MAX_POWER_VALUE 255
#define MOTOR_INERTIA_THESHOLD 190

motor *motor_init()
{
    printf("motor init\n");

    gpio_error gpioError;
    gpio_init_pwm(&gpioError);

    if (gpioError == GPIO_ERROR_INIT)
    {
        printf("gpio init error.");
        exit(1);
    }

    motor *result = (motor *)malloc(sizeof(motor));
    result->currentPower = result->prevPower = 0;
    result->direction = DIRECTION_FORWARD;
    result->started = result->starting = false;

    return result;
}

void motor_set_power(motor *incoming, int power)
{
    if (incoming == NULL)
    {
        fprintf(stderr, "invalid motor\n");
        exit(1);
    }

    printf("set power: %i. started: %i\n", power, incoming->started);

    if (power > MOTOR_MIN_POWER_INPUT && power <= MOTOR_MIN_POWER_VALUE)
    {
        power = MOTOR_MIN_POWER_VALUE;
    }

    if (incoming->starting)
    {
        printf("set power skip: starting...\n");
        return;
    }

    if (power < MOTOR_MIN_POWER_VALUE)
    {
        gpio_write_pwm(0);
        incoming->started = false;

        return;
    }

    if (power < MOTOR_INERTIA_THESHOLD && !incoming->started)
    {
        incoming->starting = true;
        printf("starting the motor:\n");
        for (int i = MOTOR_MAX_POWER_VALUE; i >= power; i -= 50)
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