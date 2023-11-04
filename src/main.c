#include <stdio.h>
#include "../libs/pigpio.h"
#include <stdbool.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/stack.h"
#include "include/gpio.h"
#include "include/bluetooth.h"
#include "include/joystick.h"
#include "include/motor.h"

static motor *pMotor;

void handle_joystick_event(joystick_event *event)
{
        printf("handle_event: type: %i; value: %i\n", event->btnType, event->value);

        if (event->btnType == JOYSTICK_BTN_RT)
        {
                if (pMotor->starting)
                {
                        return;
                }

                int currentPower = gpio_convert_controller_value(event->value);

                motor_set_power(pMotor, currentPower);
        }
}

int main()
{
        printf("Starting.\n");
        fflush(stdout);

        pMotor = motor_init();

        if (pMotor == NULL)
        {
                printf("failed to init motor");
                exit(1);
        }

        joystick_error error;
        while (1)
        {
                start_joystick_loop(handle_joystick_event, &error);

                switch (error)
                {
                case JOYSTICK_ERROR_NO_DEVICE:
                        printf("connecting bluetooth device");
                        findAndConnectBluetooth(NULL);
                        break;

                default:
                        printf("unhandled error: %i", error);
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
