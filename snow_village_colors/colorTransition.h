#ifndef _COLORTRANSITION_H
#define _COLORTRANSITION_H
#include <math.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
typedef unsigned long time_t;
typedef uint32_t color_t;
typedef uint16_t hue;
typedef uint8_t sat;
typedef uint8_t val;
typedef uint8_t lum;

typedef struct keyframe_t {
    time_t time;
    color_t color;
} keyframe_t;

typedef struct animation_segment_t {
    time_t startTime;
    keyframe_t frameTable[];
} animation_segment_t;

typedef struct sequence_handle_t{
    animation_segment_t animation_segment;
} sequence_handle_t;

void rgb_to_hsv(uint8_t red, uint8_t green, uint8_t b, float hsv[3]);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_COLORTRANSITION_H
