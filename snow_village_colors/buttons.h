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
#define BUTTON_HANDLE_TYPES 3

typedef void (*buttonActionHandler_t)(time_t);

typedef struct button_handle_t{
    bool   normalState;
    bool   perState;
    bool   curState;
    time_t debounceTimer;
    time_t holdTimer;
    int pin;
    int mode;
} button_handle_t;

typedef struct button_listener_t{
    button_handle_t * buttonHandle;
    buttonActionHandler_t handler;
}

static button_handle_t *buttonListeners[BUTTON_HANDLE_TYPES][BUTTON_MAX_LISTENERS];
static int buttonListenerIdx[] = {0, 0, 0};


void init(button_handle_t *button);

void addButtonListener(int type, button_listener_t *btnListener);
void processButtons(time_t startTime);


static void processPressButton(time_t startTime, button_listener_t*);
static void processUpButton(time_t startTime, button_listener_t*);
static void processDownButton(time_t startTime, button_listener_t*);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_BUTTONS_H
