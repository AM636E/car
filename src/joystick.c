#include <stdio.h>
#include <stdbool.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "include/joystick.h"

void start_joystick_loop(on_joystick_event handler, joystick_error *error)
{
    printf("opening joystick device.\n");
    int joyFd = open("/dev/input/js0", O_RDONLY);

    if (joyFd == -1)
    {
        perror("no joystick\n");
        return JOYSTICK_ERROR_NO_DEVICE;
    }
    struct js_event joyEvent;

    while (1)
    {
        printf("reading joystick...\n");
        if (read(joyFd, &joyEvent, sizeof(struct js_event)) == -1)
        {
            perror("cant read joystick\n");
            return JOYSTICK_ERROR_CANT_READ;
        }

        printf("joystick: n:%i; v:%i...\n", joyEvent.number, joyEvent.value);
        joystick_event event;
        event.btnType = joyEvent.number;
        event.value = joyEvent.value;
        printf("calling handler\n");
        handler(&event);
        printf("end calling handler\n");
    }
}