#include "./buttons.h"

void btn_initButton(button_handle_t *const button, int pin, int mode, buttonActionHandler_t down, buttonActionHandler_t up, buttonActionHandler_t press){
    button->pin = pin;
    button->mode = mode;
    button->downHandler = down;
    button->upHandler = up;
    button->pressHandler = press;
    button->debounceTimer = button->holdTimer = 0;
    button->preState = button->curState = button->normalState = mode == INPUT_PULLUP ? HIGH : LOW;// Not really true, user will have to set the normalState depending on pull up/down

    pinMode(pin, mode);
}

void btn_addButton(button_handle_t *const button){
    buttons[buttonsIdx++] = button;
}

void btn_processButtons(unsigned long startTime) {
    for(int i=0; i<buttonsIdx; i++){
        processButton(startTime, buttons[i]);
    }
}

static void processButton(unsigned long startTime, button_handle_t * const button){
    button->curState = digitalRead(button->pin);

    if(button->curState != button->preState && button->debounceTimer == 0) {
        button->debounceTimer = millis();
    }else if(
        button->curState != button->preState &&
        (long)(startTime - button->debounceTimer) > BUTTON_DEBOUNCE_DELAY
    ) {
        button->debounceTimer = 0;
        button->preState = button->curState;
        if(button->curState != button->normalState && button->downHandler != NULL){
            button->downHandler(startTime);
        }else if(button->curState == button->normalState){
            if(button->upHandler != NULL) button->upHandler(startTime);
            if(button->pressHandler != NULL) button->pressHandler(startTime);
        }
    }
}
