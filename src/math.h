#ifndef __MATH_H__
#define __MATH_H__

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

static u8 mul_u8_f32(u8 a, f32 b) {
    f32 result = ((f32)a) * b;
    return (u8)(255.0f < result ? 255.0f : result);
}

static f32 dot_vec3(Vec3 a, Vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static Vec3 mul_vec3_f32(Vec3 a, f32 b) {
    Vec3 result = {
        .x = a.x * b,
        .y = a.y * b,
        .z = a.z * b,
    };
    return result;
}

static Vec3 add_vec3(Vec3 a, Vec3 b) {
    Vec3 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
    return result;
}

static Vec3 sub_vec3(Vec3 a, Vec3 b) {
    Vec3 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
    return result;
}

static f32 len_vec3(Vec3 a) {
    return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

static Vec3 reflect(Vec3 a, Vec3 b) {
    Vec3 result = sub_vec3(mul_vec3_f32(b, 2.0f * dot_vec3(a, b)), a);
    return result;
}

#endif
