#include <Adafruit_NeoPixel.h>
#include "./buttons.h"
#include "./colorTransition.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define INTERNAL_LED 1
#define PIN        0
#define NUMPIXELS 8
#define DELAYVAL 500
#define BUTTON_PIN   2

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
static void noop(time_t);
static void processButtons(time_t);
static void buttonUp(time_t);
static void buttonDown(time_t);
static void buttonPress(time_t);
static void toggleLED(time_t);
static void blinkBuiltInLED(time_t);
static void colorWipe(time_t, uint32_t);
/*
static void theaterChaseRainbow(int);
static void rainbow(int);
static void theaterChase(uint32_t, int);
*/
static void interpo(uint32_t, uint32_t, time_t);
static void test(void);

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
static button_handle_t myButton;

void setup()
{
    pinMode(INTERNAL_LED, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    strip.show();  // Initialize all pixels to 'off'
    strip.begin();
    startTime = millis();

    state = STATE_ANIM_CLEAR;
    errorCode = ERROR_NONE;
    btn_initButton(&myButton, BUTTON_PIN, INPUT_PULLUP, buttonDown, toggleLED, buttonPress);
    btn_addButton(&myButton);
}

void loop()
{
    startTime = millis();
    btn_processButtons(startTime);
    animate(startTime);
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
        /*
        case STATE_ANIM_THEATER_WHITE:
            //theaterChase(WHITE, 50);
            state = STATE_PAUSE;
            break;
        case STATE_ANIM_THEATER_RED:
            //theaterChase(RED_HALF, 50);
            state = STATE_PAUSE;
            break;
        case STATE_ANIM_THEATER_BLUE:
            //theaterChase(BLUE_HALF, 50);
            state = STATE_PAUSE;
            break;
        case STATE_ANIM_THEATER_RAINBOW:
            //theaterChaseRainbow(50);
            state = STATE_PAUSE;
            break;
        case STATE_ANIM_RAINBOW:
            //rainbow(10);
            state = STATE_PAUSE;
            break;
        */
        case STATE_TEST:
            test();
            state = STATE_PAUSE;
            break;
    }
    if(errorCode != ERROR_NONE) state = STATE_ERROR;
}

static void buttonDown(time_t startTime) {}
static void noop(time_t t){}
static void buttonUp(time_t startTime){}

static void buttonPress(time_t startTime) {
    if(++buttonPressCnt >= COUNT_OF(ORDER_OF_STATES) || buttonPressCnt < 0) {
        buttonPressCnt = 0;
    }
    state = ORDER_OF_STATES[buttonPressCnt];
}

static void test()
{
    //interpo(Adafruit_NeoPixel::Color(0, 0, 0), Adafruit_NeoPixel::Color(255, 0, 0), startTime);
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

static void blinkBuiltInLED(time_t curTime, int code)
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
static void interpo(uint32_t color1, uint32_t color2, time_t time)
{
    for(int i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, color2);
    }
    strip.show();
    state = STATE_PAUSE;
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

/*
// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
static void theaterChase(uint32_t color, int wait)
{
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
static void rainbow(int wait)
{
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
static void theaterChaseRainbow(int wait)
{
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
*/
