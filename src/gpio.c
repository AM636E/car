#include <stdio.h>
#include <pigpio.h>
#include <stdbool.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/gpio.h"
#include "include/stack.h"
#include "include/joystick.h"

static bool gpioInitCalled = false;

value_range *value_range_init(int min, int max) 
{
    value_range *result = (value_range *)malloc(sizeof(value_range));
    result->min = min;
    result->max = max;

    return result;
}

void gpio_servo(int pin, int value)
{
    gpioServo(pin, value);
}

int gpio_convert_controller_value3(int incomingValue, value_range *lowRange, value_range *highRange)
{
    int newValue = ((incomingValue - highRange->min) * (lowRange->max - lowRange->min) /
                    (highRange->max - highRange->min)) +
                   lowRange->min;

    printf("gpio_convert_controller_value: newValue %i\n", newValue);

    return newValue;
}

int gpio_convert_controller_value(int incomingValue)
{
    printf("gpio_convert_controller_value: %i\n", incomingValue);
    const int old_min = JOYSTICK_INPUT_MIN;
    const int old_max = JOYSTICK_INPUT_MAX;
    const int new_min = 10;
    const int new_max = 255;

    value_range low;
    value_range high;

    low.min = new_min;
    low.max = new_max;

    high.min = old_min;
    high.max = old_max;

    return gpio_convert_controller_value3(incomingValue, &low, &high);
}

void gpio_init_pwm_pin(int pin, int frequency, gpio_error *error)
{
    printf("gpio init pin %i F:%i\n", pin, frequency);
    fflush(stdout);
    if (gpioInitCalled == false)
    {
        printf("gpio init\n"); fflush(stdout);
        if (gpioInitialise() < 0)
        {
            fprintf(stderr, "Failed to initialize GPIO\n");
            *error = GPIO_ERROR_INIT;
            return;
        }

        gpioInitCalled = true;
    }

    gpioSetPWMfrequency(pin, frequency);
}

void gpio_init_pwm(int frequency, gpio_error *error)
{
    gpio_init_pwm_pin(GPIO_PIN_PWM, frequency, error);
}

void gpio_write_pwm(gpio_pwm_value value)
{
    gpio_write_pwm_pin(GPIO_PIN_PWM, value);
}

void gpio_write_pwm_pin(int pin, gpio_pwm_value value)
{
    printf("gpio write pwm p:%i v:%i\n", pin, value);

    gpioPWM(pin, value);
}

void gpio_write(int pin, gpio_val value)
{
    gpioSetMode(pin, PI_OUTPUT);
    gpioWrite(pin, value);
}