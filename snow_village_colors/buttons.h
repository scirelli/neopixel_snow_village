#ifndef _BUTTONS_H
#define _BUTTONS_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BUTTON_DEBOUNCE_DELAY  20

typedef void (*buttonPressHandler_t)(time_t);

typedef struct button_handle {
    bool   peState = HIGH,
    bool   curState = HIGH,
    time_t changeTime;
    buttonPressHandler_t handlers[8],
    unsigned int handlers,
} button_handle_t;


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_BUTTONS_H
