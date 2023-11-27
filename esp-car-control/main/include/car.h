#ifndef CAR
#define CAR

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/mcpwm_prelude.h"

#define DIRECTION_FORWARD 1
#define DIRECTION_BACKWARD -1

typedef struct {
    uint32_t resolution_hz;
    uint32_t frequency_hz;
    int motor_min_power;
    int motor_max_power;
    int motor_speed_pin;
    int motor_direction_pin;
    int motor_forward_pin;
    int motor_backward_pin;
    int direction;
} motor_options_t;

typedef struct {
    mcpwm_cmpr_handle_t speed_comparator;
    mcpwm_cmpr_handle_t steering_comparator;
} motor_internals;

typedef struct motor_t {
    motor_options_t *options;
    motor_internals * internals;
} motor_t;

typedef motor_t * motor_handle_t ;

typedef struct car_t car_t;
typedef struct car_t {
    motor_handle_t motor;
    
    esp_err_t (*set_power)(car_t *, short power_percentage);
    esp_err_t (*set_direction)(car_t *, short direction);
    esp_err_t (*set_steering)(car_t *, short steering_position);
} car_t;

typedef car_t* car_handle_t ;

esp_err_t car_init(motor_options_t* motorOptions, car_handle_t *result);

#endif