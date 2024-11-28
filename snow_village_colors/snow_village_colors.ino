#include <time.h>
#include <Adafruit_NeoPixel.h>
#include <stdbool.h>
#include "./buttons.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define INTERNAL_LED 1
#define PIN        0
#define NUMPIXELS 8
#define DELAYVAL 500
#define BUTTON_PIN   2

#define STATE_ERROR -1
#define STATE_CLEAR     0
#define STATE_ALL_RED   1
#define STATE_ALL_GREEN 2
#define STATE_ALL_BLUE  3
#define STATE_THEATER_WHITE    4
#define STATE_THEATER_RED      5
#define STATE_THEATER_BLUE     6
#define STATE_THEATER_RAINBOW  7
#define STATE_RAINBOW 8
#define STATE_LAST 8

#define ERROR_NONE 1
#define ERROR_TEST 10

static const uint32_t BLACK = Adafruit_NeoPixel::Color(0, 0, 0);
static const uint32_t WHITE = Adafruit_NeoPixel::Color(127, 127, 127);
static const uint32_t RED = Adafruit_NeoPixel::Color(255, 0, 0);
static const uint32_t GREEN = Adafruit_NeoPixel::Color(0, 255, 0);
static const uint32_t BLUE = Adafruit_NeoPixel::Color(0, 255,   0);
static const uint32_t RED_HALF = Adafruit_NeoPixel::Color(127,   0,   0);
static const uint32_t BLUE_HALF = Adafruit_NeoPixel::Color(0,   0, 127);

typedef struct animationData_cfg{
    uint32_t frames[NUMPIXELS];
    uint16_t frameTimes[NUMPIXELS];
    uint8_t keyframe;
    const uint8_t frameCount;
    bool repeat;
} animationData_cfg_t;

typedef struct animator_handle{
    Adafruit_NeoPixel *p_strip;
    animationData_cfg_t *animData;
    uint32_t frameIndex;
    double elapsed;
    void (*reset)(animator_handle_t*, double dt);
    void (*update)(animator_handle_t*, double dt);
} animator_handle_t;

static void animatorReset(animator_handle_t *, double);
static int colorWipeAnimation(animator_handle_t*, double);
static void processButtons(time_t);

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
static Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
static bool   buttonPreState = HIGH, buttonCurState = HIGH;
static int    state     = 0;
static time_t startTime, buttonChangeTime;
static double dif;
static int    errorCode = ERROR_NONE;
static animationData_cfg_t animData = {
    {RED , BLACK, GREEN, BLACK, BLUE, BLACK, BLACK, BLACK},
    {1000, 10   , 1000 , 10   , 1000, 10   , 1000 , 10   },
    0, NUMPIXELS, false
};
static animator_handle_t animator = {
    &strip,
    &animData,
    0, 0.0, &animatorReset, &colorWipeAnimation
};

void setup() {
    pinMode(INTERNAL_LED, OUTPUT);
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    strip.show();  // Initialize all pixels to 'off'
    strip.begin();
    startTime, buttonDown, buttonUp = time(NULL);

    state = STATE_CLEAR;
    buttonPreState = HIGH;
    buttonCurState = HIGH;
    errorCode = ERROR_NONE;
}

void loop() {
    dif=difftime(time(NULL),startTime);
    startTime = time(NULL);

    processButtons(startTime);

    switch(state) {
        case STATE_CLEAR:
            strip.clear();
            strip.show();
            break;
        case STATE_ALL_RED:
            errorCode = colorWipeAnimation(&animator_handle_t, dif);
            break;
        case STATE_ALL_GREEN:
            colorWipe(GREEN, 50);
            break;
        case STATE_ALL_BLUE:
            colorWipe(BLUE, 50);
            break;
        case STATE_THEATER_WHITE:
            theaterChase(WHITE, 50);
            break;
        case STATE_THEATER_RED:
            theaterChase(RED_HALF, 50);
            break;
        case STATE_THEATER_BLUE:
            theaterChase(BLUE_HALF, 50);
            break;
        case STATE_THEATER_RAINBOW:
            theaterChaseRainbow(50);
            break;
        case STATE_RAINBOW:
            rainbow(10);
            break;
        case STATE_ERROR:
            displayErrorCode(errorCode, dif);
            break;
    }
    if(errorCode != ERROR_NONE) state = STATE_ERROR;
}

static void processButtons(time_t startTime) {
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

static void displayErrorCode(int errorCode, double dt) {
    for(int i=0; i<errorCode; i++) {
        analogWrite(INTERNAL_LED, 255);
        delay(200);
        analogWrite(INTERNAL_LED, 0);
        delay(200);
    }
    delay(300);
    for(int i=0; i<3; i++) {
        analogWrite(INTERNAL_LED, 255);
        delay(20);
        analogWrite(INTERNAL_LED, 0);
        delay(20);
    }
    delay(500);
}

static void animatorReset(animator_handle_t *p_anim, double dt) {
    p_anim->frameIndex = 0;
    p_anim->elapsed = 0.0;
}

static int colorWipeAnimation(animator_handle_t* p_anim, double dt) {
    strip.setPixelColor(p_anim->frameIndex, p_anim->animData->frames[p_anim->frameIndex]);
    strip.show();
    return ERROR_NONE;
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
static void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
static void theaterChase(uint32_t color, int wait) {
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
static void rainbow(int wait) {
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
static void theaterChaseRainbow(int wait) {
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
