#ifndef __MATH_H__
#define __MATH_H__

#include "types.h"

static u8 saturate_add_u8(u8 a, u8 b) {
    return (MAX_U8 - b) < a ? (u8)MAX_U8 : (u8)(a + b);
}

static u8 saturate_sub_u8(u8 a, u8 b) {
    return a <= b ? (u8)0 : (u8)(a - b);
}

static u8 saturate_mul_u8(u8 a, u8 b) {
    u16 a_16 = (u16)a;
    u16 b_16 = (u16)b;
    u16 result = (u16)(a_16 * b_16);
    return result < (u16)MAX_U8 ? (u8)result : MAX_U8;
}

#endif
