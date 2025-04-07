#include "color.h"

static int median_three(int a, int b, int c)
{
    if ((a <= b && b <= c) || (c <= b && b <= a)) {
        return b;
    } else if ((b <= a && a <= c) || (c <= a && a <= b)) {
        return a;
    } else {
        return c;
    }
}

/**
 * Converts RGB to HSV. Scans hue degrees 0 to 1.
*/
void rgb_to_hsv(uint8_t red, uint8_t green, uint8_t blue, float hsv[3])
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

void rgb_to_hsv_intv1(uint8_t red, uint8_t green, uint8_t blue, int hsv_int[3])
{
    int maxVal = red;
    if (green > maxVal) maxVal = green;
    if (blue > maxVal) maxVal = blue;

    int minVal = red;
    if (green < minVal) minVal = green;
    if (blue < minVal) minVal = blue;

    int delta = maxVal - minVal;

    int hue = 0;
    int saturation = 0;
    int value = maxVal;

    if (delta == 0) {
        hue = 0;
    } else {
        if (maxVal == red) {
            hue = (green - blue);
            if (hue < 0) hue += 6 * 255;
        } else if (maxVal == green) {
            hue = 2 * 255 + (blue - red);
        } else { // maxVal == blue
            hue = 4 * 255 + (red - green);
            if (hue < 0) hue += 6 * 255; // Should not happen
        }
        hue = (hue * 360) / (6 * 255);
    }

    if (maxVal == 0) {
        saturation = 0;
    } else {
        saturation = (delta * 255) / maxVal;
    }

    hsv_int[0] = hue;
    hsv_int[1] = saturation;
    hsv_int[2] = value;
}

void rgb_to_hsv_intv2(uint8_t R, uint8_t G, uint8_t B, uint16_t hsv[3])
{
    uint16_t E = 65535;
    int16_t bitShift = 16;

    // Step 1: find the min, max, and median of RGB
    int16_t m = R;
    if (G < m) m = G;
    if (B < m) m = B;

    int16_t M = R;
    if (G > M) M = G;
    if (B > M) M = B;

    int16_t c = median_three(R, G, B);

    // Step 2: Set V equal to M
    hsv[2] = M;

    // Step 3: calculate the difference between M and m, if its 0, set S to 0, and H to -1 (It is undefined in this case)
    int16_t d = M - m;
    if (d == 0 || hsv[2] == 0) {
        hsv[1] = 0;
        hsv[0] = -1;
        return;
    }

    // Step 4: calculate S
    hsv[1] = (uint16_t)((((uint32_t)d << bitShift) - 1) / hsv[2]); // Using uint64_t to avoid overflow

    // Step 5: find the selector index I
    int16_t I = -1;
    if (M == R && m == B) {
        I = 0;
    } else if (M == G && m == B) {
        I = 1;
    } else if (M == G && m == R) {
        I = 2;
    } else if (M == B && m == R) {
        I = 3;
    } else if (M == B && m == G) {
        I = 4;
    } else if (M == R && m == G) {
        I = 5;
    }
    // Fallback to ensure I is set if there are duplicate min/max values
    if (I == -1) {
        if (M == R) I = (m == G) ? 5 : 0;
        else if (M == G) I = (m == B) ? 1 : 2;
        else if (M == B) I = (m == R) ? 3 : 4;
    }

    // Step 6: calculate F
    int16_t F = (int16_t)(((uint32_t)(c - m) << 16) / d + 1);
    if (I % 2 != 0) {
        F = E - F;
    }

    // Step 7: calculate H
    hsv[0] = (E * I) + F;
}

//Converts an integer RGB (value range from 0 to 255) to an HSV
void rgb_to_hsv_intv3(uint8_t r, uint8_t g, uint8_t b, uint8_t hsv[3])
{
    //Compute the H value by finding the maximum of the RGB values
    // Step 1: find the min, max, and median of RGB
    uint8_t m = MIN3(r,g,b);
    uint8_t M = MAX3(r,g,b);
    uint8_t d = M - m;

    //Compute the value
    hsv[2] = M;
    if(hsv[2] == 0) {
      hsv[0] = hsv[1] = 0;
      return;
    }

    //Compute the saturation value
    hsv[1] = 255 * (long)d / hsv[2];

    if(hsv[1] == 0) {
        hsv[0] = 0;
        return;
    }

    // Compute the Hue
    if(M == r) {
        hsv[0] = 0 + 43*(g - b)/d;
    }else if(M == g) {
        hsv[0] = 85 + 43*(b - r)/d;
    }else{//M == b
        hsv[0] = 171 + 43*(r - b)/d;
    }
}
