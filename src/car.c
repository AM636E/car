#include <stdio.h>
#include <stdlib.h>

#include "include/motor.h"
#include "include/car.h"
#include "include/gpio.h"
#include "include/joystick.h"

car *car_init(motor_options *options, value_range *steeringRange)
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
    result->steeringServoRange = steeringRange;
    
    return result;
}

void car_turn_raw(car *incoming, int incomingValue)
{
    gpio_servo(STEERING_PIN, incomingValue);
}

void car_turn(car *incoming, int incomingValue)
{
    const int old_min = JOYSTICK_INPUT_MIN;
    const int old_max = JOYSTICK_INPUT_MAX;

    value_range high;

    high.min = old_min;
    high.max = old_max;

    int gpioValue = gpio_convert_controller_value3(-incomingValue, incoming->steeringServoRange, &high);

    printf("convert steering value: %i. Result: [%i]\n", incomingValue, gpioValue);

    car_turn_raw(incoming, gpioValue);
}