#include <stdio.h>
#include "../libs/pigpio.h"
#include <stdbool.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/gpio.h"
#include "include/stack.h"

stack *convertRange3(int incomingValue, value_range *lowRange, value_range *highRange)
{
    stack *result = initialize();

    if (result == NULL)
    {
        fprintf(stderr, "Failed to allocate memory");
    }

    int new_value = ((incomingValue - highRange->min) * (lowRange->max - lowRange->min) /
                     (highRange->max - highRange->min)) +
                    lowRange->min;

    printf("new_value %i\n", new_value);

    if (incomingValue <= 100)
    {
        int *max = (int *)malloc(sizeof(int));
        (*max) = 255;

        for (int i = new_value; i < 255; i += 50)
        {
            printf("push %i\n", i);
            int *val = (int *)malloc(sizeof(int));
            (*val) = i;
            push(result, val);
        }
        push(result, max);
        push(result, max);
        push(result, max);
    }

    if (incomingValue < 40)
    {
        push(result, NULL);
    }

    return result;
}

stack *convertRange(int incomingValue)
{
    printf("convert range: %i\n", incomingValue);
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

    return convertRange3(incomingValue, &low, &high);
}

void gpio_init_pwm(gpio_error *error)
{
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "Failed to initialize GPIO\n");
        *error = GPIO_ERROR_INIT;
        return;
    }

    gpioSetPWMfrequency(GPIO_PIN_PWM, GPIO_DEFAULT_PWM_FREQUENCY);
}

void gpio_write_pwm(gpio_pwm_value value)
{
    gpioPWM(GPIO_PIN_PWM, value);
}