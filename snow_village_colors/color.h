#ifndef _COLOR_H
#define _COLOR_H
#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))
#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))

void rgb_to_hsv(uint8_t red, uint8_t green, uint8_t b, float hsv[3]);
void rgb_to_hsv_intv1(uint8_t r, uint8_t g, uint8_t b, int hsv[3]);
void rgb_to_hsv_intv2(uint8_t R, uint8_t G, uint8_t B, uint16_t hsv[3]);
void rgb_to_hsv_intv3(uint8_t r, uint8_t g, uint8_t b, uint8_t hsv[3]);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_COLOR_H
