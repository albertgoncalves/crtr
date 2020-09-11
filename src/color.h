#ifndef __COLOR_H__
#define __COLOR_H__

typedef struct {
    u8 blue;
    u8 red;
    u8 green;
} RgbColor;

#define U8_MAX 0xFFu

static u8 saturate_add_u8(u8 a, u8 b) {
    return (U8_MAX - b) < a ? (u8)U8_MAX : (u8)(a + b);
}

static RgbColor add_color(RgbColor a, RgbColor b) {
    RgbColor result = {
        .red = saturate_add_u8(a.red, b.red),
        .green = saturate_add_u8(a.green, b.green),
        .blue = saturate_add_u8(a.blue, b.blue),
    };
    return result;
}

#endif
