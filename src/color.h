#ifndef __COLOR_H__
#define __COLOR_H__

#include "math.h"

typedef struct {
    u8 blue;
    u8 red;
    u8 green;
} rgbColor;

static rgbColor add_color(rgbColor a, rgbColor b) {
    rgbColor c = {
        .red = saturate_add_u8(a.red, b.red),
        .green = saturate_add_u8(a.green, b.green),
        .blue = saturate_add_u8(a.blue, b.blue),
    };
    return c;
}

static rgbColor sub_color(rgbColor a, rgbColor b) {
    rgbColor c = {
        .red = saturate_sub_u8(a.red, b.red),
        .green = saturate_sub_u8(a.green, b.green),
        .blue = saturate_sub_u8(a.blue, b.blue),
    };
    return c;
}

static rgbColor mul_color(rgbColor a, u8 k) {
    rgbColor b = {
        .red = saturate_mul_u8(a.red, k),
        .green = saturate_mul_u8(a.green, k),
        .blue = saturate_mul_u8(a.blue, k),
    };
    return b;
}

static rgbColor div_color(rgbColor a, u8 k) {
    rgbColor b = {
        .red = (u8)(a.red / k),
        .green = (u8)(a.green / k),
        .blue = (u8)(a.blue / k),
    };
    return b;
}

#endif
