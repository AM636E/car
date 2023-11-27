#include "driver/gpio.h"

#include "include/car.h"

#define TAG "[CAR]"

esp_err_t set_car_power(car_handle_t car_handle, short power_percentage)
{
    int low = car_handle->motor->options->motor_min_power;
    int high = car_handle->motor->options->motor_max_power;

    double new_value = (((high - low) * (power_percentage - 0)) / (100.0 - 0)) + low;

    ESP_LOGI(TAG, "set power %i->[%f]", power_percentage, new_value);
    return mcpwm_comparator_set_compare_value(car_handle->motor->internals->speed_comparator, new_value);
}

esp_err_t set_car_direction(car_handle_t car_handle, short direction)
{
    if (car_handle == NULL)
    {
        ESP_LOGE(TAG, "car handle is NULL");

        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "set car direction %p %i", car_handle, direction);
    switch (direction)
    {
    case DIRECTION_BACKWARD:
        gpio_set_level(car_handle->motor->options->motor_backward_pin, 1);
        gpio_set_level(car_handle->motor->options->motor_forward_pin, 0);
        break;

    case DIRECTION_FORWARD:
        gpio_set_level(car_handle->motor->options->motor_backward_pin, 0);
        gpio_set_level(car_handle->motor->options->motor_forward_pin, 1);
        break;

    default:
        break;
    }

    return ESP_OK;
}

esp_err_t set_car_steering(car_handle_t car_handle, short steering_position)
{
    if (car_handle == NULL)
    {
        ESP_LOGE(TAG, "car handle is NULL");

        return ESP_ERR_INVALID_STATE;
    }
    int min = 500;
    int max = 2500;
    int dmin = -90;
    int dmax = 90;

    int position = (steering_position - dmin) * (min - max) / (dmax - dmin) + min;
    if (position < 0)
    {
        position = -position;
    }

    ESP_LOGI(TAG, "set steering: %i->[%i]", steering_position, position);
    return mcpwm_comparator_set_compare_value(car_handle->motor->internals->steering_comparator, position);
}

#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000 // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD 20000          // 20000 ticks, 20ms

esp_err_t car_init_steering(car_handle_t car_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Create timer and operator");
    mcpwm_timer_handle_t timer_handle = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };

    ESP_GOTO_ON_ERROR(mcpwm_new_timer(&timer_config, &timer_handle), err, TAG, "failed to create steering timer");

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };
    ESP_LOGI(TAG, "Steering: creating steering operator");
    ESP_GOTO_ON_ERROR(mcpwm_new_operator(&operator_config, &oper), err, TAG, "steering operator failed");

    ESP_LOGI(TAG, "Steering: Connect timer and operator");
    ESP_GOTO_ON_ERROR(mcpwm_operator_connect_timer(oper, timer_handle), err, TAG, "failed to connect steering timer");

    ESP_LOGI(TAG, "Steering: Create comparator and generator from the steering operator");
    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_GOTO_ON_ERROR(mcpwm_new_comparator(oper, &comparator_config, &comparator), err, TAG, "failed to create comparator");

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = car_handle->motor->options->motor_direction_pin,
    };
    ESP_LOGI(TAG, "Steering: create steering generator");
    ESP_GOTO_ON_ERROR(mcpwm_new_generator(oper, &generator_config, &generator), err, TAG, "failed to create steering generator");

    ESP_LOGI(TAG, "Steering: Set generator action on timer and compare event");
    // go high on counter empty
    ESP_GOTO_ON_ERROR(mcpwm_generator_set_action_on_timer_event(generator,
                                                                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)),
                      err, TAG, "steering generator failed");

    // go low on compare threshold
    ESP_GOTO_ON_ERROR(mcpwm_generator_set_action_on_compare_event(generator,
                                                                  MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)),
                      err, TAG, "steering generator failed");

    car_handle->motor->internals->steering_comparator = comparator;

    car_handle->set_steering = &set_car_steering;

    car_handle->set_steering(car_handle, 0);

    ESP_LOGI(TAG, "Enable steering timer");
    ESP_GOTO_ON_ERROR(mcpwm_timer_enable(timer_handle), err, TAG, "failed to enable timer");

    ESP_LOGI(TAG, "start steering timer");
    ESP_GOTO_ON_ERROR(mcpwm_timer_start_stop(timer_handle, MCPWM_TIMER_START_NO_STOP), err, TAG, "failed to start timer");

    return ESP_OK;
err:
    ESP_LOGE(TAG, "init steering failed %i", ret);

    if (oper != NULL)
    {
        mcpwm_del_operator(oper);
    }

    if (generator != NULL)
    {
        mcpwm_del_generator(generator);
    }

    if (comparator != NULL)
    {
        mcpwm_del_comparator(comparator);
    }

    return ret;
}

esp_err_t car_init_direction(car_handle_t car_handle)
{
    if (car_handle == NULL)
    {
        ESP_LOGE(TAG, "car handle is NULL");

        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "car init direction %i|%i", car_handle->motor->options->motor_forward_pin, car_handle->motor->options->motor_backward_pin);

    esp_err_t ret;
    motor_options_t *motor_options = car_handle->motor->options;

    if (!GPIO_IS_VALID_OUTPUT_GPIO(motor_options->motor_forward_pin))
    {
        ESP_LOGE(TAG, "GPIO pin %i is not a valid output pin", motor_options->motor_forward_pin);
        return ESP_ERR_INVALID_STATE;
    }

    if (!GPIO_IS_VALID_OUTPUT_GPIO(motor_options->motor_backward_pin))
    {
        ESP_LOGE(TAG, "GPIO pin %i is not a valid output pin", motor_options->motor_backward_pin);
        return ESP_ERR_INVALID_STATE;
    }

    gpio_config_t gpio_config_data = {
        .pin_bit_mask = 1ULL << motor_options->motor_forward_pin,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE};

    ESP_LOGI(TAG, "cat init forward pin");
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_config_data), err, TAG, "failed to config pin %i", motor_options->motor_forward_pin);

    ESP_LOGI(TAG, "cat init backward pin");
    gpio_config_data.pin_bit_mask = 1ULL << motor_options->motor_backward_pin; // pin_to_mask(motor_options->motor_backward_pin);
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_config_data), err, TAG, "failed to config pin %i", motor_options->motor_backward_pin);

    set_car_direction(car_handle, DIRECTION_FORWARD);

    return ESP_OK;
err:

    return ret;
}

esp_err_t car_init(motor_options_t *motor_options, car_handle_t *result)
{
    ESP_LOGI(TAG, "Car init %lu|%lu", motor_options->frequency_hz, motor_options->resolution_hz);

    esp_err_t ret = ESP_OK;
    // Configure MCPWM timer
    mcpwm_timer_config_t timer_config = {
        .group_id = 0, // Assuming using MCPWM group 0
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = motor_options->resolution_hz,
        .period_ticks = motor_options->resolution_hz / motor_options->frequency_hz,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };

    ESP_LOGI(TAG, "Create timer");

    mcpwm_timer_handle_t timer_handle;
    ESP_GOTO_ON_ERROR(mcpwm_new_timer(&timer_config, &timer_handle), cleanup, TAG, "create timer failed");

    ESP_LOGI(TAG, "Create operator");
    // Configure MCPWM operator
    mcpwm_operator_config_t operator_config = {
        .group_id = 0 // Same group as the timer
    };
    mcpwm_oper_handle_t operator_handle;
    ESP_GOTO_ON_ERROR(mcpwm_new_operator(&operator_config, &operator_handle), cleanup, TAG, "create operator failed");

    ESP_LOGI(TAG, "Connect timer");
    // Connect the operator to the timer
    ESP_GOTO_ON_ERROR(mcpwm_operator_connect_timer(operator_handle, timer_handle), cleanup, TAG, "connect timer failed");

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };

    ESP_LOGI(TAG, "Create speed comparator");
    static mcpwm_cmpr_handle_t speed_comparator_handle;
    ESP_GOTO_ON_ERROR(mcpwm_new_comparator(operator_handle, &comparator_config, &speed_comparator_handle), cleanup, TAG, "comparator failed");
    ESP_GOTO_ON_ERROR(mcpwm_comparator_set_compare_value(speed_comparator_handle, 50), cleanup, TAG, "set value failed");

    ESP_LOGI(TAG, "Create generator");
    // Configure MCPWM generator
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = 7,
    };
    mcpwm_gen_handle_t generator_handle;
    ESP_GOTO_ON_ERROR(mcpwm_new_generator(operator_handle, &generator_config, &generator_handle), cleanup, TAG, "create generator failed");

    ESP_LOGI(TAG, "Generator set actions HIGH");
    ESP_GOTO_ON_ERROR(mcpwm_generator_set_actions_on_timer_event(generator_handle,
                                                                 MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                                                                 MCPWM_GEN_TIMER_EVENT_ACTION_END()),
                      cleanup, TAG, "failed to set HIGH event");

    ESP_LOGI(TAG, "Generator set actions LOW");
    ESP_GOTO_ON_ERROR(mcpwm_generator_set_actions_on_compare_event(generator_handle,
                                                                   MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, generator_handle, MCPWM_GEN_ACTION_LOW),
                                                                   MCPWM_GEN_COMPARE_EVENT_ACTION_END()),
                      cleanup, TAG, "failed to set LOW event");

    motor_handle_t motor = (motor_handle_t)malloc(sizeof(motor_t));
    motor_internals *internals = (motor_internals *)malloc(sizeof(motor_internals));
    internals->speed_comparator = speed_comparator_handle;
    motor->internals = internals;
    motor->options = motor_options;

    *result = (car_handle_t)malloc(sizeof(car_t));
    (*result)->motor = motor;
    (*result)->set_power = &set_car_power;
    (*result)->set_direction = &set_car_direction;

    car_init_steering(*result);

    // Start the MCPWM timer

    ESP_LOGI(TAG, "Enable timer");
    ESP_GOTO_ON_ERROR(mcpwm_timer_enable(timer_handle), cleanup, TAG, "failed to enable timer");

    ESP_LOGI(TAG, "Enable timer");
    ESP_GOTO_ON_ERROR(mcpwm_timer_start_stop(timer_handle, MCPWM_TIMER_START_NO_STOP), cleanup, TAG, "failed to start timer");

    ESP_GOTO_ON_ERROR(car_init_direction(*result), cleanup, TAG, "failed to init direction");

    return ret;
cleanup:
    if (generator_handle != NULL)
    {
        mcpwm_del_generator(generator_handle);
    }
    if (speed_comparator_handle != NULL)
    {
        mcpwm_del_comparator(speed_comparator_handle);
    }
    if (operator_handle != NULL)
    {
        mcpwm_del_operator(operator_handle);
    }
    if (timer_handle != NULL)
    {
        mcpwm_del_timer(timer_handle);
    }

    return ret;
}