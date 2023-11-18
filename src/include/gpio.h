#ifndef GPIO
#define GPIO
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

typedef int gpio_error;
typedef unsigned char gpio_pwm_value;

typedef int gpio_pin_num;

typedef bool gpio_val;

#define HIGH true
#define LOW false

#define GPIO_PIN_PWM 18
#define GPIO_DEFAULT_PWM_FREQUENCY 100

#define GPIO_ERROR_INIT 1

typedef struct
{
    int min;
    int max;
} value_range;

value_range *value_range_init(int min, int max);

void gpio_servo(int pin, int value);

int gpio_convert_controller_value3(int incomingValue, value_range *lowRange, value_range *highRange);

int gpio_convert_controller_value(int controllerValue);

void gpio_init_pwm(int frequency, gpio_error *error);

void gpio_init_pwm_pin(int pin, int frequency, gpio_error *error);

void gpio_write_pwm(gpio_pwm_value value);

void gpio_write_pwm_pin(int pin, gpio_pwm_value value);

void gpio_write(int pin, gpio_val value);

#endif