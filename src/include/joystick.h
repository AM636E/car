#ifndef JOYSTICK
#define JOYSTICK

#define JOYSTICK_BTN_RT 4
#define JOYSTICK_BTN_LT 5
#define JOYSTICK_TIT_V1 0
#define JOYSTICK_TIT_V2 1

#define JOYSTICK_ERROR_NO_DEVICE -1
#define JOYSTICK_ERROR_CANT_READ 1

typedef int joystick_error;

typedef int btn_type;

typedef struct joystick_event
{
    btn_type btnType;
    int value;
} joystick_event;

typedef void (*on_joystick_event)(joystick_event *event);

void start_joystick_loop(on_joystick_event eventHandler, joystick_error *error);

#endif