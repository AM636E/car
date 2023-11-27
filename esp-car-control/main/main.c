#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "led_strip.h"

#include "include/car.h"
#include "include/tune.h"

#define TAG "[MAIN]"

#define BDC_MCPWM_TIMER_RESOLUTION_HZ 10000000
#define BDC_MCPWM_FREQ_HZ 25000
#define MOTOR_MIN_POWER 300
#define MOTOR_MAX_POWER 400

static car_handle_t car;

static led_strip_handle_t led_handle;

#define LED_NUM 250

static int value = 1;
static int count = 1;

static int r = 0;
static int g = 0;
static int b = 0;

static int steering = 0;

static int direction = DIRECTION_FORWARD;
static void pid_loop_cb(void *args)
{
    value++;
    count++;
    steering += 20;
    if (count > LED_NUM)
    {
        count = 0;
    }
    if (value > 100)
    {
        value = 0;
    }

    if (value % 20 == 0)
    {
      //  direction *= -1;
        steering *= -1;
        car->set_direction(car, direction);
    }

    if (steering > 90 || steering < -90)
    {
        steering = 0;
    }

    //  car->set_power(car, 5);
    car->set_steering(car, steering);

    r = (r + 5) % 255;
    g = (g + 20) % 255;
    b = (b + 10) % 255;

    for (int i = 0; i < count; i++)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_handle, i, (r + i) % 255, (g + i) % 255, (b + i) % 255));
    }
    for (int i = LED_NUM - 1; i >= LED_NUM - count; i--)
    {
        ESP_ERROR_CHECK(led_strip_set_pixel(led_handle, i, 0, 0, 0));
    }
    /* Refresh the strip to send data */
    ESP_ERROR_CHECK(led_strip_refresh(led_handle));
}

int min(int a, int b) { return a > b ? b : a; }

esp_err_t app_main()
{
    esp_err_t ret = ESP_OK;

    led_config_t config = {
        .led_count = LED_NUM,
        .led_pin = 36,
        .led_resolution = (10 * 1000 * 1000)};

    led_handle = led_configure(&config);

    motor_options_t options = {
        .resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ,
        .frequency_hz = BDC_MCPWM_FREQ_HZ,
        .motor_max_power = MOTOR_MAX_POWER,
        .motor_min_power = MOTOR_MIN_POWER,
        .motor_speed_pin = 7,
        .motor_direction_pin = 6,
        .motor_forward_pin = 8,
        .motor_backward_pin = 9};

    ESP_GOTO_ON_ERROR(car_init(&options, &car), err, TAG, "car init failed");

    ESP_LOGI(TAG, "Create a timer to do POWER calculation periodically");
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = pid_loop_cb,
        .name = "pid_loop"};
    esp_timer_handle_t pid_loop_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &pid_loop_timer));
    ESP_LOGI(TAG, "Start motor speed loop");
    ESP_ERROR_CHECK(esp_timer_start_periodic(pid_loop_timer, 200 * 1000));

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

led_strip_handle_t led_configure(led_config_t *config)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = config->led_pin,        // The GPIO that connected to the LED strip's data line
        .max_leds = config->led_count,            // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src = RMT_CLK_SRC_DEFAULT,          // different clock source can lead to different power consumption
        .resolution_hz = config->led_resolution, // RMT counter clock frequency
        .flags.with_dma = false,                 // DMA feature is available on ESP target like ESP32-S3
#endif
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}