#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* NOTE: See `https://www.gabrielgambetta.com/computer-graphics-from-scratch/`.
 * NOTE: See `https://www.youtube.com/watch?v=pq7dV4sR7lg`.
 */

#define WIDTH  1024
#define HEIGHT 1152
#define SIZE   1310720

#define WIDTH_FLOAT       1024.0f
#define HEIGHT_FLOAT      1152.0f
#define HALF_WIDTH_FLOAT  512.0f
#define HALF_HEIGHT_FLOAT 576.0f

#define EPSILON 0.01f

#define N_SPHERES 4
#define N_LIGHTS  3

#define VIEWPORT_SIZE      1.0f
#define PROJECTION_PLANE_Z 1.0f
#define MIN_DISTANCE       1.0f

#define REFLECT_DEPTH 4

#define FILEPATH "out/main.bmp"

#include "bmp.h"

typedef struct {
    vec3     center;
    rgbColor color;
    f32      radius;
    f32      specular;
    f32      reflective;
} sphere;

typedef enum {
    FALSE = 0,
    TRUE,
} bool;

typedef struct {
    f32  threshold;
    u8   index;
    bool exists;
} intersectionResult;

typedef enum {
    AMBIENT = 0,
    PINPOINT,
    DIRECTIONAL,
} lightType;

typedef struct {
    lightType type;
    f32       intensity;
    vec3      position;
} lightSource;

static vec3 CAMERA_POSITION = {.x = 0.0f, .y = 0.0f, .z = -2.0f};

static rgbColor BACKGROUND = {.red = 245, .green = 245, .blue = 245};

static sphere SPHERES[N_SPHERES] = {
    {.center = {.x = 0.0f, .y = -1.0f, .z = 3.0f},
     .radius = 1.0f,
     .color = {.red = 85, .green = 240, .blue = 160},
     .specular = 500.0f,
     .reflective = 0.2f},
    {.center = {.x = 2.0f, .y = 0.0f, .z = 4.0f},
     .radius = 1.0f,
     .color = {.red = 240, .green = 160, .blue = 85},
     .specular = 500.0f,
     .reflective = 0.4f},
    {.center = {.x = -2.0f, .y = 0.0f, .z = 4.0f},
     .radius = 1.0f,
     .color = {.red = 160, .green = 85, .blue = 240},
     .specular = 10.f,
     .reflective = 0.3f},
    {.center = {.x = 0.0f, .y = -5001.0f, .z = 0.0f},
     .radius = 5000.0f,
     .color = {.red = 128, .green = 128, .blue = 128},
     .specular = 1000.0f,
     .reflective = 0.1f},
};

static lightSource LIGHTS[N_LIGHTS] = {
    {.type = AMBIENT, .intensity = 0.2f, .position = {0}},
    {.type = PINPOINT,
     .intensity = 0.6f,
     .position = {.x = 2.0f, .y = 1.0f, .z = 0.0f}},
    {.type = DIRECTIONAL,
     .intensity = 0.2f,
     .position = {.x = 1.0f, .y = 4.0f, .z = 4.0f}},
};

static u8 mul_u8_f32(u8 a, f32 b) {
    f32 result = ((f32)a) * b;
    return (u8)(255.0f < result ? 255.0f : result);
}

static intersectionResult nearest_intersection(vec3 origin,
                                               vec3 direction,
                                               f32  min_distance,
                                               f32  max_distance) {
    intersectionResult result = {
        .threshold = F32_MAX,
        .index = 0,
        .exists = FALSE,
    };
    for (u8 i = 0; i < N_SPHERES; ++i) {
        sphere s = SPHERES[i];
        vec3   center = sub_vec3(origin, s.center);
        f32    k1 = dot_vec3(direction, direction);
        f32    k2 = 2.0f * dot_vec3(center, direction);
        f32    k3 = dot_vec3(center, center) - (s.radius * s.radius);
        f32    discriminant = (k2 * k2) - (4.0f * k1 * k3);
        if (EPSILON < discriminant) {
            f32 t1 = (-k2 - sqrtf(discriminant)) / (2.0f * k1);
            f32 t2 = (-k2 + sqrtf(discriminant)) / (2.0f * k1);
            if ((t1 < result.threshold) && (min_distance < t1) &&
                (t1 < max_distance)) {
                result.threshold = t1;
                result.index = i;
                result.exists = TRUE;
            }
            if ((t2 < result.threshold) && (min_distance < t2) &&
                (t2 < max_distance)) {
                result.threshold = t2;
                result.index = i;
                result.exists = TRUE;
            }
        }
    }
    return result;
}

static vec3 reflect(vec3 a, vec3 b) {
    vec3 result = sub_vec3(mul_vec3_f32(b, 2.0f * dot_vec3(a, b)), a);
    return result;
}

static f32 light_intensity(vec3 point, vec3 normal, vec3 view, f32 specular) {
    f32 intensity = 0.0f;
    f32 len_normal = len_vec3(normal);
    f32 len_view = len_vec3(view);
    for (u8 i = 0; i < N_LIGHTS; ++i) {
        lightSource l = LIGHTS[i];
        if (l.type == AMBIENT) {
            intensity += l.intensity;
        } else {
            vec3 position;
            f32  t;
            if (l.type == PINPOINT) {
                position = sub_vec3(l.position, point);
                t = 1.0;
            } else {
                position = l.position;
                t = F32_MAX;
            }
            intersectionResult shadow =
                nearest_intersection(point, position, EPSILON, t);
            if (shadow.exists == TRUE) {
                continue;
            }
            f32 reflection_diffuse = dot_vec3(normal, position);
            if (0.0f < reflection_diffuse) {
                intensity += (l.intensity * reflection_diffuse) /
                             (len_normal * len_vec3(position));
            }
            vec3 reflection_specular = reflect(normal, position);
            f32  reflection = dot_vec3(reflection_specular, view);
            if (0.0f < reflection) {
                intensity += l.intensity *
                             powf(reflection / (len_vec3(reflection_specular) *
                                                len_view),
                                  specular);
            }
        }
    }
    return intensity;
}

typedef struct {
    rgbColor color;
    f32      reflective;
} colorReflection;

static void render(pixel* pixels) {
    vec3 camera_direction = {
        .x = 0.0f,
        .y = 0.0f,
        .z = PROJECTION_PLANE_Z,
    };
    colorReflection reflections[REFLECT_DEPTH] = {0};
    f32             min_distance = MIN_DISTANCE;
    for (f32 y = -HALF_HEIGHT_FLOAT; y < HALF_HEIGHT_FLOAT; ++y) {
        camera_direction.y = (y * VIEWPORT_SIZE) / HEIGHT_FLOAT;
        for (f32 x = -HALF_WIDTH_FLOAT; x < HALF_WIDTH_FLOAT; ++x) {
            camera_direction.x = (x * VIEWPORT_SIZE) / WIDTH_FLOAT;
            vec3 ray_position = CAMERA_POSITION;
            vec3 ray_direction = camera_direction;
            u8   index = 0;
            for (u8 i = 0; i < REFLECT_DEPTH; ++i) {
                intersectionResult intersection =
                    nearest_intersection(ray_position,
                                         ray_direction,
                                         min_distance,
                                         F32_MAX);
                if (intersection.exists == TRUE) {
                    sphere s = SPHERES[intersection.index];
                    vec3   point = add_vec3(
                        ray_position,
                        mul_vec3_f32(ray_direction, intersection.threshold));
                    vec3 normal = sub_vec3(point, s.center);
                    normal = mul_vec3_f32(normal, 1.0f / len_vec3(normal));
                    vec3 view = mul_vec3_f32(ray_direction, -1.0f);
                    f32  intensity =
                        light_intensity(point, normal, view, s.specular);
                    colorReflection* r = &reflections[index++];
                    r->color.red = mul_u8_f32(s.color.red, intensity);
                    r->color.blue = mul_u8_f32(s.color.blue, intensity);
                    r->color.green = mul_u8_f32(s.color.green, intensity);
                    r->reflective = s.reflective;
                    if (s.reflective <= 0.0f) {
                        break;
                    }
                    ray_position = point;
                    ray_direction = reflect(view, normal);
                    min_distance = EPSILON;
                } else {
                    break;
                }
            }
            if (0 < index) {
                for (u8 i = (u8)(index - 1); 0 < i; --i) {
                    u8       j = (u8)(i - 1);
                    rgbColor reflection = reflections[i].color;
                    rgbColor color = reflections[j].color;
                    f32      reflective = reflections[j].reflective;
                    color.red = mul_u8_f32(color.red, 1.0f - reflective);
                    color.green = mul_u8_f32(color.green, 1.0f - reflective);
                    color.blue = mul_u8_f32(color.blue, 1.0f - reflective);
                    reflection.red = mul_u8_f32(reflection.red, reflective);
                    reflection.green =
                        mul_u8_f32(reflection.green, reflective);
                    reflection.blue = mul_u8_f32(reflection.blue, reflective);
                    reflections[j].color = add_color(color, reflection);
                }
                rgbColor color = reflections[0].color;
                pixels->red = color.red;
                pixels->green = color.green;
                pixels->blue = color.blue;
            } else {
                pixels->red = BACKGROUND.red;
                pixels->green = BACKGROUND.green;
                pixels->blue = BACKGROUND.blue;
            }
            ++pixels;
        }
    }
}

int main(void) {
    fileHandle* file = fopen(FILEPATH, "wb");
    if (file == NULL) {
        return EXIT_FAILURE;
    }
    bmpBuffer* buffer = calloc(sizeof(bmpBuffer), 1);
    if (buffer == NULL) {
        return EXIT_FAILURE;
    }
    set_bmp_header(&buffer->bmp_header);
    set_dib_header(&buffer->dib_header);
    render(buffer->pixels);
    write_bmp(file, buffer);
    fclose(file);
    free(buffer);
}
