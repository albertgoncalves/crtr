#ifndef __COLOR_H__
#define __COLOR_H__

#include "math.h"

typedef struct {
    u8 blue;
    u8 red;
    u8 green;
} RgbColor;

static RgbColor add_color(RgbColor a, RgbColor b) {
    RgbColor result = {
        .red = saturate_add_u8(a.red, b.red),
        .green = saturate_add_u8(a.green, b.green),
        .blue = saturate_add_u8(a.blue, b.blue),
    };
    return result;
}

#endif
