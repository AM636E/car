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

int convert_controller_value(int incomingValue, value_range *lowRange, value_range *highRange)
{
    int newValue = ((incomingValue - highRange->min) * (lowRange->max - lowRange->min) /
                    (highRange->max - highRange->min)) + lowRange->min;

    printf("gpio_convert_controller_value: newValue %i\n", newValue);

    return newValue;
}

int gpio_convert_controller_value(int incomingValue)
{
    printf("gpio_convert_controller_value: %i\n", incomingValue);
    const int old_min = -32767;
    const int old_max = 32767;
    const int new_min = 10;
    const int new_max = 255;

    value_range low;
    value_range high;

    low.min = new_min;
    low.max = new_max;

    high.min = old_min;
    high.max = old_max;

    return convert_controller_value(incomingValue, &low, &high);
}

void gpio_init_pwm(int frequency, gpio_error *error)
{
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Failed to initialize GPIO\n");
        *error = GPIO_ERROR_INIT;
        return;
    }

    gpioSetPWMfrequency(GPIO_PIN_PWM, frequency);
}

void gpio_write_pwm(gpio_pwm_value value)
{
    gpioPWM(GPIO_PIN_PWM, value);
}

void gpio_write(int pin, gpio_val value)
{
    gpioSetMode(pin, PI_OUTPUT);
    gpioWrite(pin, value);
}