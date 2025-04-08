#ifndef _BUTTONS_H
#define _BUTTONS_H
#include <stdbool.h>
#include "Arduino.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BUTTON_DEBOUNCE_DELAY  20
#define BUTTON_MAX_BUTTONS 1

#define BUTTON_MAX_LISTENERS  16
#define BUTTON_LISTENER_PRESS 0
#define BUTTON_LISTENER_DOWN  1
#define BUTTON_LISTENER_UP    2
#define BUTTON_LISTENER_TYPES 3

typedef void (*buttonActionHandler_t)(unsigned long);

typedef struct button_handle_t{
    bool   normalState;
    bool   preState;
    bool   curState;
    unsigned long debounceTimer;
    unsigned long holdTimer;
    int pin;
    int mode;
    buttonActionHandler_t pressHandler;
    buttonActionHandler_t upHandler;
    buttonActionHandler_t downHandler;
} button_handle_t;

static button_handle_t *buttons[BUTTON_MAX_BUTTONS];
static uint8_t buttonsIdx = 0;

void btn_initButton(button_handle_t *const button, int pin, int mode, buttonActionHandler_t down, buttonActionHandler_t up, buttonActionHandler_t press);
void btn_addButton(button_handle_t * const button);
void btn_processButtons(unsigned long startTime);


static void processButton(unsigned long startTime, button_handle_t * const button);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_BUTTONS_H
