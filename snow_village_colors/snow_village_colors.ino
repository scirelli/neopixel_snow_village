#include <time.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        0
#define NUMPIXELS 8
#define DELAYVAL 500
#define BUTTON_PIN   2

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

const uint32_t BLACK = Adafruit_NeoPixel::Color(0, 0, 0);
const uint32_t WHITE = Adafruit_NeoPixel::Color(127, 127, 127);
const uint32_t RED = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t GREEN = Adafruit_NeoPixel::Color(0, 255, 0);
const uint32_t BLUE = Adafruit_NeoPixel::Color(0, 255,   0);
const uint32_t RED_HALF = Adafruit_NeoPixel::Color(127,   0,   0);
const uint32_t BLUE_HALF = Adafruit_NeoPixel::Color(0,   0, 127);

typedef struct AnimationData {
    uint32_t color[NUMPIXELS];
    uint8_t frameTimes[NUMPIXELS];
    uint8_t frameCount = NUMPIXELS;
} AnimationData;

typedef struct Animator {
    Adafruit_NeoPixel *p_strip;
    AnimationData *p_anim;
    uint32_t frameIndex;
    uint32_t wait;
    double elapsed;
    void (*reset)(Animator*);
} Animator

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
boolean oldState = HIGH;
int     state     = 0;    // Currently-active animation state, 0-9
time_t start,end;
double dif;
double duration=40.0;
ColorWipe red = { RED, 50, 0.0, p_strip };

void setup() {
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    strip.show();  // Initialize all pixels to 'off'
    strip.begin();
    end = start = time(NULL);
}

void loop() {
    start = time(NULL);
    dif=difftime(end,start);
    // Get current button state.
    boolean newState = digitalRead(BUTTON_PIN);

    // Check if state changed from high to low (button press).
    if((newState == LOW) && (oldState == HIGH)) {
        // Short delay to debounce button.
        delay(20);
        // Check if button is still low after debounce.
        newState = digitalRead(BUTTON_PIN);
        if(newState == LOW) {      // Yes, still low
            if(++state > STATE_LAST) state = STATE_CLEAR; // Advance to next state, wrap around after #8
        }
    }
    // Set the last-read button state to the old state.
    oldState = newState;

    switch(state) {
        case STATE_CLEAR:
            colorWipe(BLACK, 50);
            break;
        case STATE_ALL_RED:
            colorWipe(RED, 50);
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
    }
    end = time(NULL);
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
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
void rainbow(int wait) {
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
void theaterChaseRainbow(int wait) {
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
