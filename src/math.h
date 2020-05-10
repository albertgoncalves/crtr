#ifndef __MATH_H__
#define __MATH_H__

#include "types.h"

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} vec3;

static u8 saturate_add_u8(u8 a, u8 b) {
    return (U8_MAX - b) < a ? (u8)U8_MAX : (u8)(a + b);
}

static f32 dot_vec3(vec3 a, vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static vec3 mul_vec3_f32(vec3 a, f32 b) {
    vec3 result = {
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b,
    };
    return result;
}

static vec3 add_vec3(vec3 a, vec3 b) {
    vec3 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
    return result;
}

static vec3 sub_vec3(vec3 a, vec3 b) {
    vec3 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
    return result;
}

static f32 len_vec3(vec3 a) {
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

#endif
