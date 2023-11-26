#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "include/car.h"
#define TAG "[MAIN]"

#define BDC_MCPWM_TIMER_RESOLUTION_HZ 10000000
#define BDC_MCPWM_FREQ_HZ 25000
#define MOTOR_MIN_POWER 300
#define MOTOR_MAX_POWER 400

static car_handle_t car;

static int value = 1;
static int direction = DIRECTION_FORWARD;
static void pid_loop_cb(void *args)
{
    value++;
    
    if (value > 100)
    {
        value = 0;
    }

    if(value % 20 == 0) {
        direction *= -1;
        car->set_direction(car, direction);
    }

    car->set_power(car, value);
}

esp_err_t app_main(void)
{
    esp_err_t ret = ESP_OK;
    motor_options_t options = {
        .resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ,
        .frequency_hz = BDC_MCPWM_FREQ_HZ,
        .motor_max_power = MOTOR_MAX_POWER,
        .motor_min_power = MOTOR_MIN_POWER,
        .motor_speed_pin = 7,
        .motor_direction_pin = 6,
        .motor_forward_pin = 8,
        .motor_backward_pin = 9
    };

    ESP_GOTO_ON_ERROR(car_init(&options, &car), err, TAG, "car init failed");

    ESP_LOGI(TAG, "Create a timer to do POWER calculation periodically");
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = pid_loop_cb,
        .name = "pid_loop"};
    esp_timer_handle_t pid_loop_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &pid_loop_timer));
    ESP_LOGI(TAG, "Start motor speed loop");
    ESP_ERROR_CHECK(esp_timer_start_periodic(pid_loop_timer, 200 * 1000));

    int v = 0;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return ret;
err:
    if (car)
    {
        free(car);
    }

    return ret;
}
