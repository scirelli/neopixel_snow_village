#include "./colorTransition.h"
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
