#include <Adafruit_NeoPixel.h>
/* #include "color.h" */
/* #include "buttons.h" */

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define PIN       0
#define NUMPIXELS 8
#define DELAYVAL 500
#define BUTTON_PIN 2
#define BUTTON_DEBOUNCE_DELAY  20

#define STATE_PAUSE                -3
#define STATE_ERROR                -2
#define STATE_ANIM_ALL_RED          0
#define STATE_ANIM_ALL_GREEN        1
#define STATE_ANIM_ALL_BLUE         2
#define STATE_ANIM_THEATER_WHITE    3
#define STATE_ANIM_THEATER_RED      4
#define STATE_ANIM_THEATER_BLUE     5
#define STATE_ANIM_THEATER_RAINBOW  6
#define STATE_ANIM_RAINBOW          7
#define STATE_ANIM_CLEAR            8
#define STATE_TEST                  9

#define ERROR_NONE 1
#define ERROR_TEST 10

typedef unsigned long time_t;
typedef uint32_t color_t;
typedef uint16_t hue_t;
typedef uint8_t sat_t;
typedef uint8_t val_t;
typedef uint8_t lum_t;

static const int8_t ORDER_OF_STATES[] = {
    STATE_ANIM_CLEAR,
    STATE_TEST,
    STATE_ANIM_ALL_RED,
    STATE_ANIM_ALL_GREEN,
    STATE_ANIM_ALL_BLUE,
    /*
    STATE_ANIM_THEATER_WHITE,
    STATE_ANIM_THEATER_RED,
    STATE_ANIM_THEATER_BLUE,
    STATE_ANIM_THEATER_RAINBOW,
    STATE_ANIM_RAINBOW,
    */
};

static void animate(time_t);
static void buttonPress(time_t);
static void buttonUp(time_t);
static void buttonDown(time_t);
static void toggleLED(time_t);
static void test(void);
static void colorWipe(time_t, uint32_t);
static void blinkBuiltInLED(unsigned long, int);

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
static Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
static time_t startTime;
static int8_t state = ORDER_OF_STATES[0];
//static button_handle_t myButton;
static uint8_t buttonPressCnt = 0; //Index into the ORDER_OF_STATES
static int8_t errorCode = ERROR_NONE;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif

/*
    strip.show();  // Initialize all pixels to 'off'
    strip.begin();
    startTime = millis();

    state = ORDER_OF_STATES[0];
    errorCode = ERROR_NONE;

    btn_initButton(&myButton, BUTTON_PIN, INPUT_PULLUP, buttonDown, toggleLED, buttonPress);
    btn_addButton(&myButton);
    */
}

void loop()
{
    startTime = millis();
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(1000);
    //btn_processButtons(startTime);
    //animate(startTime);
}

static void animate(time_t startTime)
{
    switch(state) {
        case STATE_ERROR:
            blinkBuiltInLED(startTime, errorCode);
            break;
        case STATE_PAUSE:
            delay(10);
            break;
        case STATE_ANIM_CLEAR:
            strip.clear();
            strip.show();
            state = STATE_PAUSE;
            break;
        case STATE_ANIM_ALL_RED:
            colorWipe(startTime, Adafruit_NeoPixel::Color(255, 0, 0));
            break;
        case STATE_ANIM_ALL_GREEN:
            colorWipe(startTime, Adafruit_NeoPixel::Color(0, 255, 0));
            break;
        case STATE_ANIM_ALL_BLUE:
            colorWipe(startTime, Adafruit_NeoPixel::Color(0, 0, 255));
            break;
        case STATE_TEST:
            test();
            state = STATE_PAUSE;
            break;
    }
    if(errorCode != ERROR_NONE) state = STATE_ERROR;
}

static void buttonUp(time_t startTime){}
static void buttonDown(time_t startTime){}

static void buttonPress(time_t startTime)
{
    if(++buttonPressCnt >= COUNT_OF(ORDER_OF_STATES) || buttonPressCnt < 0) {
        buttonPressCnt = 0;
    }
    state = ORDER_OF_STATES[buttonPressCnt];
}

static void test()
{
    hue_t h = 0;
    sat_t s = 255;
    val_t v = 255;
    color_t col;

    col = strip.ColorHSV(h, s, v);
    strip.setPixelColor(0, strip.gamma32(col));

    /*
    rgb_to_hsv_intv1(0, 255, 0, hsv);
    col = strip.ColorHSV((hue)hsv[0], (sat)hsv[1], (val)hsv[2]);
    strip.setPixelColor(1, strip.gamma32(col));

    rgb_to_hsv_intv1(0, 0, 255, hsv);
    col = strip.ColorHSV((hue)hsv[0], (sat)hsv[1], (val)hsv[2]);
    strip.setPixelColor(2, strip.gamma32(col));
    */
    strip.show();
}

static void blinkBuiltInLED(unsigned long curTime, int code)
{
    static unsigned long d = 0;
    unsigned long delta =  curTime - d;

    if(d == 0){
        digitalWrite(LED_BUILTIN, HIGH);
        d = millis();
    }else if(delta >= 1000 && delta <= 2000){
        digitalWrite(LED_BUILTIN, LOW);
    }else if(delta > 2000) {
        d = 0;
    }
}

static void toggleLED(time_t _)
{
    static bool t = LOW;
    t = !t;
    digitalWrite(LED_BUILTIN, t);
}

static void colorWipe(time_t t, uint32_t color)
{
    static int wait = 0, i=0;
    if(wait == 0){
        strip.setPixelColor(i, color);
        strip.show();
        wait = t;
        if(++i>=strip.numPixels()){
            i=0;
            state = STATE_PAUSE;
            wait = 0;
        }
    }
    if((t - wait) > 50) wait = 0;
}
