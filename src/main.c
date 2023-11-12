#include <stdio.h>
#include <stdbool.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/pigpio.h"
#include "include/stack.h"
#include "include/gpio.h"
#include "include/bluetooth.h"
#include "include/joystick.h"
#include "include/motor.h"
#include "include/car.h"

#define MOTOR_MIN_POWER_INPUT 40
#define MOTOR_MIN_POWER_VALUE 50
#define MOTOR_MAX_POWER_VALUE 255
#define MOTOR_INERTIA_THESHOLD 190
#define MOTOR_PIN_FORWARD 23
#define MOTOR_PIN_BACKWARD 24

static car *pCar;

void handle_joystick_event(joystick_event *event)
{
        printf("handle_event: type: %i; value: %i\n", event->btnType, event->value);

        switch (event->btnType)
        {
        case JOYSTICK_TIT_V1:
        {
                car_turn(pCar, event->value);
                break;
        }
        case JOYSTICK_BTN_RT:
        {
                if (pCar->carMotor->starting)
                {
                        return;
                }
                motor_set_direction(pCar->carMotor, DIRECTION_FORWARD);
                int currentPower = gpio_convert_controller_value(event->value);

                motor_set_power(pCar->carMotor, currentPower);

                break;
        }
        case JOYSTICK_BTN_LT:
        {
                if (pCar->carMotor->starting)
                {
                        return;
                }
                motor_set_direction(pCar->carMotor, DIRECTION_BACKWARD);
                int currentPower = gpio_convert_controller_value(event->value);

                motor_set_power(pCar->carMotor, currentPower);
                break;
        }
        default:
                break;
        }
}

motor_options *get_default_options()
{
        motor_options *options = (motor_options *)malloc(sizeof(motor_options));
        options->inertiaTreshold = MOTOR_INERTIA_THESHOLD;
        options->minPowerInput = MOTOR_MIN_POWER_INPUT;
        options->minPowerValue = MOTOR_MIN_POWER_VALUE;
        options->maxPowerInput = MOTOR_MAX_POWER_VALUE;
        options->pwmFrequency = GPIO_DEFAULT_PWM_FREQUENCY;
        options->forwardSignalPin = MOTOR_PIN_FORWARD;
        options->backwardSignalPin = MOTOR_PIN_BACKWARD;

        return options;
}

motor_options *get_options(int argc, char **argv)
{
        motor_options *options = (motor_options *)malloc(sizeof(motor_options));
        options->inertiaTreshold = MOTOR_INERTIA_THESHOLD;
        options->minPowerInput = MOTOR_MIN_POWER_INPUT;
        options->minPowerValue = MOTOR_MIN_POWER_VALUE;
        options->maxPowerInput = MOTOR_MAX_POWER_VALUE;
        options->pwmFrequency = GPIO_DEFAULT_PWM_FREQUENCY;
        options->forwardSignalPin = MOTOR_PIN_FORWARD;
        options->backwardSignalPin = MOTOR_PIN_BACKWARD;

        return options;
}

int main(int argc, char *argv[])
{
        printf("Starting. argc: %i\n", argc);
        fflush(stdout);

        motor_options *options = get_default_options();

        for (int i = 0; i < argc; i++)
        {
                printf((argv[i]));
                if (strcmp(argv[i], "-f") == 0)
                {
                        printf((argv[i]));
                        options->pwmFrequency = atoi(argv[i + 1]);
                }

                if (strcmp(argv[i], "-t") == 0)
                {
                        options->inertiaTreshold = atoi(argv[i + 1]);
                }

                if (strcmp(argv[i], "-imin") == 0)
                {
                        options->minPowerInput = atoi(argv[i + 1]);
                }

                if (strcmp(argv[i], "-pmax") == 0)
                {
                        options->maxPowerInput = atoi(argv[i + 1]);
                }

                if (strcmp(argv[i], "-pmin") == 0)
                {
                        options->minPowerValue = atoi(argv[i + 1]);
                }
        }

        pCar = car_init(options);

        if (pCar == NULL)
        {
                fprintf(stderr, "failed to init car");
                exit(1);
        }

        // car_turn_raw(pCar, 1500);
        // sleep(1);

        // car_turn_raw(pCar, 1000);
        // sleep(1);
        
        // car_turn_raw(pCar, 2500);
        // sleep(1);
        
        // car_turn_raw(pCar, 2000);
        // sleep(1);
        
        // car_turn_raw(pCar, 500);
        // sleep(1);

        // car_turn_raw(pCar, 2000);
        // sleep(5);
        
        // car_turn_raw(pCar, 2500);
        // sleep(5);

        joystick_error error;
        while (1)
        {
                start_joystick_loop(handle_joystick_event, &error);

                switch (error)
                {
                case JOYSTICK_ERROR_NO_DEVICE:
                        fprintf(stderr, "connecting bluetooth device");
                        findAndConnectBluetooth(NULL);
                        break;

                default:
                        printf("unhandled error: %i\n", error);
                        break;
                }
                sleep(1);
        }
}

/*
 if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize GPIO\n");
        return 1;
    }

    int duty_cycle = 0;
    int param = atoi(argv[1]);

    int joy_fd;
    struct js_event joy_event;

    joy_fd = open("/dev/input/js0", O_RDONLY);

        if(joy_fd == -1) {
        {
                perror("no joystick");
        }
        return 1;
    }
    bool started = 0;
        bool processing = 0;

    // Set up PWM on GPIO 18 with a frequency of 1kHz and 50% duty cycle
    gpioSetPWMfrequency(18, 1000);  // Set frequency to 1kHz
        while(1) {
                if(read(joy_fd, &joy_event, sizeof(struct js_event)) == -1) {
                        perror("cant read");
                        return 1;
                }
      // Print the event type, number, and value
        printf("Event: type %d, time %d, number %d, value %d",
               joy_event.type, joy_event.time, joy_event.number, joy_event.value);


                printf("\n");
        }

 // Set dduty cycle. 128 in range 0-255 for 50% duty cycle

    getchar();  // Wait for user input

    gpioPWM(18, 0);  // Stop PWM
    gpioTerminate();  // Release resources and terminate library

    return 0;
*/
