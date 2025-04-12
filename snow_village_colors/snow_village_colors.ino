#include <Adafruit_NeoPixel.h>
//#include "color.h"
#include "buttons.h"

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
static void test(time_t);
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
static int8_t state = ORDER_OF_STATES[0];
static button_handle_t myButton;
static uint8_t buttonPressCnt = 0; //Index into the ORDER_OF_STATES
static int8_t errorCode = ERROR_NONE;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif

    strip.show();  // Initialize all pixels to 'off'
    strip.begin();

    /* state = ORDER_OF_STATES[0]; */
    /* errorCode = ERROR_NONE; */

    /* btn_initButton(&myButton, BUTTON_PIN, INPUT_PULLUP, buttonDown, toggleLED, buttonPress); */
    /* btn_addButton(&myButton); */
}

void loop()
{
    /* time_t startTime = millis(); */
    /* btn_processButtons(startTime); */
    /* animate(startTime); */

    static uint16_t counter = 0;

    strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(counter++, 255, 255)));
    strip.show();
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
            test(startTime);
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

static void test(time_t curTime)
{
    static uint16_t colors[361] = {0,182,364,546,728,910,1092,1274,1456,1638,1820,2002,2185,2367,2549,2731,2913,3095,3277,3459,3641,3823,4005,4187,4369,4551,4733,4915,5097,5279,5461,5643,5825,6007,6189,6371,6554,6736,6918,7100,7282,7464,7646,7828,8010,8192,8374,8556,8738,8920,9102,9284,9466,9648,9830,10012,10194,10376,10558,10740,10923,11105,11287,11469,11651,11833,12015,12197,12379,12561,12743,12925,13107,13289,13471,13653,13835,14017,14199,14381,14563,14745,14927,15109,15292,15474,15656,15838,16020,16202,16384,16566,16748,16930,17112,17294,17476,17658,17840,18022,18204,18386,18568,18750,18932,19114,19296,19478,19661,19843,20025,20207,20389,20571,20753,20935,21117,21299,21481,21663,21845,22027,22209,22391,22573,22755,22937,23119,23301,23483,23665,23847,24030,24212,24394,24576,24758,24940,25122,25304,25486,25668,25850,26032,26214,26396,26578,26760,26942,27124,27306,27488,27670,27852,28034,28216,28399,28581,28763,28945,29127,29309,29491,29673,29855,30037,30219,30401,30583,30765,30947,31129,31311,31493,31675,31857,32039,32221,32403,32585,32768,32950,33132,33314,33496,33678,33860,34042,34224,34406,34588,34770,34952,35134,35316,35498,35680,35862,36044,36226,36408,36590,36772,36954,37137,37319,37501,37683,37865,38047,38229,38411,38593,38775,38957,39139,39321,39503,39685,39867,40049,40231,40413,40595,40777,40959,41141,41323,41506,41688,41870,42052,42234,42416,42598,42780,42962,43144,43326,43508,43690,43872,44054,44236,44418,44600,44782,44964,45146,45328,45510,45692,45875,46057,46239,46421,46603,46785,46967,47149,47331,47513,47695,47877,48059,48241,48423,48605,48787,48969,49151,49333,49515,49697,49879,50061,50244,50426,50608,50790,50972,51154,51336,51518,51700,51882,52064,52246,52428,52610,52792,52974,53156,53338,53520,53702,53884,54066,54248,54430,54613,54795,54977,55159,55341,55523,55705,55887,56069,56251,56433,56615,56797,56979,57161,57343,57525,57707,57889,58071,58253,58435,58617,58799,58982,59164,59346,59528,59710,59892,60074,60256,60438,60620,60802,60984,61166,61348,61530,61712,61894,62076,62258,62440,62622,62804,62986,63168,63351,63533,63715,63897,64079,64261,64443,64625,64807,64989,65171,65353,65535};
    static time_t s = 0;
    static uint16_t counter = 0;

    /* if(s == 0) s = curTime; */
    /* if((curTime - s) >= 200){ */
    /*     s = 0; */
    /*     counter += 182; */
    /* } */

    strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(counter++, 255, 255)));
    strip.show();
    /*
    //red
    strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(colors[0], 255, 255)));
    //green
    strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(colors[120], 255, 255)));
    //blue
    strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(colors[240], 255, 255)));
    //white
    strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(0, 0, 255)));
    */
}

static void blinkBuiltInLED(time_t curTime, int errorCode)
{
    static time_t d = 0;
    time_t delta =  curTime - d;

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
