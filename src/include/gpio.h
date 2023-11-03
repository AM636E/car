#ifndef GPIO
#define GPIO
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

typedef int gpio_error;
typedef unsigned char gpio_pwm_value;

typedef int gpio_pin_num;

#define GPIO_PIN_PWM 18
#define GPIO_DEFAULT_PWM_FREQUENCY 1000

#define GPIO_ERROR_INIT 1

typedef struct
{
    short min;
    short max;
} value_range;

stack *convertRange(int controllerValue);

void gpio_init_pwm(gpio_error *error);

void gpio_write_pwm(gpio_pwm_value value);

#endif