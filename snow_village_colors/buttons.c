#include "./buttons.h"

void initButton(button_handle_t *button){
    pinMode(button->pin, button->mode);
    button->perState = button->curState = button->normalState = button->mode == INPUT_PULLUP ? HIGH : LOW;// Not really true, user will have to set the normalState depending on pull up/down
}

void addButtonListener(int type, button_listener_t *btnListener) {
    if(type < 0 || type >= BUTTON_LISTENER_TYPES || buttonListenerIdx[type] >= BUTTON_MAX_LISTENERS){
        return;
    }
    buttonListeners[type][buttonListenerIdx[type]++] = btnListener;
}

void processButtons(time_t startTime) {
    for(int t=0; t<BUTTON_LISTENER_TYPES; t++){
        for(int j=0; j<buttonListenerIdx[t]; j++){
            switch(t){
            case BUTTON_LISTENER_PRESS:
                processPressButton(startTime, buttonListeners[t][j]);
                break;
            case BUTTON_LISTENER_DOWN:
                processDownButton(startTime, buttonListeners[t][j]);
                break;
            case BUTTON_LISTENER_UP:
                processUpButton(startTime, buttonListeners[t][j]);
                break;
            }
        }
    }
}

static void processPressButton(time_t starTime, button_listener_t *buttonListener){
    buttonListener->curState = digitalRead(buttonListener->pin);

    if(buttonListener->curState != buttonListener->preState && buttonListener->debounceTimer == 0) {
        buttonListener->debounceTimer = time(NULL);
    }
    if(
        buttonListener->curState != buttonListener->preState &&
        buttonListener->debounceTimer != 0 &&
        difftime(startTime, buttonListener->debounceTime) > BUTTON_DEBOUNCE_DELAY
    ) {
        buttonListener->debounceTimer = 0;
        buttonListener->preState = buttonListener->curState;
    }

    if(buttonChangeTime != 0 && difftime(startTime, buttonChangeTime) >= BUTTON_DEBOUNCE_DELAY) {
        if(buttonCurState != buttonPreState) {
            if(++state > STATE_LAST) state = STATE_CLEAR; // Advance to next state, wrap around after #8
        }
        buttonPreState = buttonCurState;
        buttonChangeTime = 0;
    }
}

static void processUpButton(time_t starTime, button_listener_t *buttonListener){
}

static void processDownButton(time_t starTime, button_listener_t *buttonListener){
}
