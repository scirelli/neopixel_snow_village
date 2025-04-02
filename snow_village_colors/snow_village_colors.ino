#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define INTERNAL_LED 1
#define PIN        0
#define NUMPIXELS 8
#define DELAYVAL 500
#define BUTTON_PIN   2
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
typedef uint16_t hue;
typedef uint8_t sat;
typedef uint8_t val;
typedef uint8_t lum;

static const int ORDER_OF_STATES[] = {
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
static void processButton(time_t);
static void buttonPress(time_t);
//static void toggleLED(time_t);
static void colorWipe(time_t, uint32_t);
static void test(void);
static void rgb_to_hsv(uint8_t red, uint8_t green, uint8_t b, float hsv[3]);

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
static int    state = ORDER_OF_STATES[0];
static int    buttonPressCnt = 0; //Index into the ORDER_OF_STATES
static int    errorCode = ERROR_NONE;
static bool   btnPrevState = false;

void setup()
{
    pinMode(INTERNAL_LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    strip.show();  // Initialize all pixels to 'off'
    strip.begin();
    startTime = millis();

    state = STATE_ANIM_CLEAR;
    errorCode = ERROR_NONE;
}

void loop()
{
    startTime = millis();
    processButton(startTime);
    animate(startTime);
}

static void animate(time_t startTime)
{
    switch(state) {
        case STATE_ERROR:
            //blinkBuiltInLED(startTime, errorCode);
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

static void processButton(unsigned long startTime){
    static unsigned long debounceTimer = 0;
    bool btnCurState = digitalRead(BUTTON_PIN);

    if(btnCurState != btnPrevState && debounceTimer == 0) {
        debounceTimer = millis();
    }else if(
        btnCurState != btnPrevState &&
        (long)(startTime - debounceTimer) > BUTTON_DEBOUNCE_DELAY
    ) {
        debounceTimer = 0L;
        btnPrevState = btnCurState;
        if(btnCurState == true){
            buttonPress(startTime);
        }
    }
}

static void buttonPress(time_t startTime) {
    if(++buttonPressCnt >= COUNT_OF(ORDER_OF_STATES) || buttonPressCnt < 0) {
        buttonPressCnt = 0;
    }
    state = ORDER_OF_STATES[buttonPressCnt];
}

static void test()
{
    float hsv[3];
    color_t col;

    rgb_to_hsv(255, 0, 0, hsv);
    hsv[0] = hsv[0] * 65536;
    hsv[1] = hsv[1] * 256;
    hsv[2] = hsv[2] * 256;
    col = strip.ColorHSV((hue)hsv[0], (sat)hsv[1], (val)hsv[2]);
    strip.setPixelColor(0, strip.gamma32(col));

    rgb_to_hsv(0, 255, 0, hsv);
    hsv[0] = hsv[0] * 65536;
    hsv[1] = hsv[1] * 256;
    hsv[2] = hsv[2] * 256;
    col = strip.ColorHSV((hue)hsv[0], (sat)hsv[1], (val)hsv[2]);
    strip.setPixelColor(1, strip.gamma32(col));

    rgb_to_hsv(0, 0, 255, hsv);
    hsv[0] = hsv[0] * 65536;
    hsv[1] = hsv[1] * 256;
    hsv[2] = hsv[2] * 256;
    col = strip.ColorHSV((hue)hsv[0], (sat)hsv[1], (val)hsv[2]);
    strip.setPixelColor(2, strip.gamma32(col));

    strip.show();
}

/*
static void toggleLED(time_t _)
{
    static bool t = LOW;
    t = !t;
    digitalWrite(LED_BUILTIN, t);
}
*/

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

/**
 * Converts RGB to HSV. Scans hue degrees 0 to 1.
 */
static void rgb_to_hsv(uint8_t red, uint8_t green, uint8_t blue, float hsv[3])
{
    float rp = (float)red/255.f,
          gp = (float)green/255.f,
          bp = (float)blue/255.f,
          Cmax = 0.f,
          Cmin = 0.f,
          delta = 0.f,
          hue = 0.f;

    Cmax = rp > gp ? rp : gp;
    Cmax = Cmax < bp ? bp : Cmax;
    Cmin = rp < gp ? rp : gp;
    Cmin = Cmin > gp ? gp : Cmin;
    delta = Cmax - Cmin;

    hsv[2] = Cmax;

    if(delta == 0){
        hue = 0;
    } else if(Cmax == rp) {
        hue = 60 * ( fmodf(((gp - bp)/delta), 6.f) );
    } else if(Cmax = gp) {
        hue = 60 * ( ((bp - rp)/delta) + 2 );
    } else if(Cmax = bp) {
        hue = 60 * ( ((rp - gp)/delta) + 4 );
    }

    if(Cmax == 0) {
        hsv[1] = 0;
    }else {
        hsv[1] = delta/Cmax;
    }

    hsv[0] = hue/360;
}
