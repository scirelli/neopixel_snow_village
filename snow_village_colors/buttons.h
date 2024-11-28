#ifndef _BUTTONS_H
#define _BUTTONS_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BUTTON_DEBOUNCE_DELAY  20

#define BUTTON_MAX_LISTENERS  16
#define BUTTON_LISTENER_PRESS 0
#define BUTTON_LISTENER_DOWN  1
#define BUTTON_LISTENER_UP    2
#define BUTTON_LISTENER_TYPES 3

typedef void (*buttonActionHandler_t)(time_t);

typedef struct button_listener_t{
    bool   peState = HIGH;
    bool   curState = HIGH;
    time_t changeTime;
    int pin;
    buttonActionHandler_t handler;
} button_listener_t;

static button_listener_t buttonListeners[BUTTON_LISTENER_TYPES][BUTTON_MAX_LISTENERS];
static int buttonListenerIdx[] ={0, 0, 0};

void addButtonListener(int type, button_listener_t *btnListener) {
}

void processButtons(time_t startTime) {
    buttonCurState = digitalRead(BUTTON_PIN);
    if(buttonCurState != buttonPreState && buttonChangeTime == 0) {
        buttonChangeTime = time(NULL);
    }
    if(buttonChangeTime != 0 && difftime(startTime, buttonChangeTime) >= BUTTON_DEBOUNCE_DELAY) {
        if(buttonCurState != buttonPreState) {
            if(++state > STATE_LAST) state = STATE_CLEAR; // Advance to next state, wrap around after #8
        }
        buttonPreState = buttonCurState;
        buttonChangeTime = 0;
    }
}


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_BUTTONS_H
