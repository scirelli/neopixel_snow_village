#include <stdio.h>
#include "color.h"

int main(void)
{
    uint8_t hsv[3];

    rgb_to_hsv_intv3(128, 128, 128, hsv);
    printf("(%d,%d,%d)\n", hsv[0], hsv[1], hsv[2]);
    rgb_to_hsv_intv3(255, 0, 255, hsv);
    printf("(%d,%d,%d)\n", hsv[0], hsv[1], hsv[2]);

    return 0;
}
