#include <stdio.h>
#include <stdlib.h>

#include "include/motor.h"
#include "include/car.h"
#include "include/gpio.h"

car *car_init(motor_options *options)
{
    printf("car_init\n");
    gpio_error gpioError;
    gpio_init_pwm_pin(STEERING_PIN, options->pwmFrequency, &gpioError);

    if (gpioError == GPIO_ERROR_INIT)
    {
        printf("gpio init error.");
        exit(1);
    }

    motor *motor = motor_init(options);

    car *result = (car *)malloc(sizeof(car));
    result->carMotor = motor;
    result->steeringPosition = 0;

    return result;
}

void car_turn_raw(car *incoming, int incomingValue)
{
    gpio_servo(STEERING_PIN, incomingValue);
}

void car_turn(car *incoming, int incomingValue)
{
    const int old_min = -32767;
    const int old_max = 32767;
    const int new_min = 800;
    const int new_max = 2200;

    value_range low;
    value_range high;

    low.min = new_min;
    low.max = new_max;

    high.min = old_min;
    high.max = old_max;

    int gpioValue = gpio_convert_controller_value3(incomingValue, &low, &high);

    printf("convert steering value: %i. Result: [%i]\n", incomingValue, gpioValue);

    car_turn_raw(incoming, gpioValue);
}