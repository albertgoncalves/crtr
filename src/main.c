#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* NOTE: See `https://www.gabrielgambetta.com/computer-graphics-from-scratch/`.
 * NOTE: See `https://www.youtube.com/watch?v=pq7dV4sR7lg`.
 */

#define WIDTH  1024
#define HEIGHT 1024
#define SIZE   1048576

#define WIDTH_FLOAT  1024.0f
#define HEIGHT_FLOAT 1024.0f

#define FILEPATH "out/main.png"

#include "bmp.h"

typedef struct {
    vec3     normal;
    f32      delta;
    rgbColor color;
} plane;

typedef struct {
    vec3     center;
    f32      radius;
    rgbColor color;
} sphere;

#define EPSILON 0.0001f

#define N_PLANES  1
#define N_SPHERES 1

static rgbColor BACKGROUND = {.red = 240, .green = 240, .blue = 240};

static plane PLANES[N_PLANES] = {
    {.normal = {.x = 0.0f, .y = 0.0f, .z = 1.0f},
     .delta = 0.0f,
     .color = {.red = 255, .green = 0, .blue = 255}},
};

static sphere SPHERES[N_SPHERES] = {
    {.center = {.x = 0.5f, .y = -2.0f, .z = 1.25f},
     .radius = 2.0f,
     .color = {.red = 0, .green = 255, .blue = 255}},
};

static rgbColor cast_ray(vec3 ray_origin, vec3 ray_direction) {
    f32      threshold = F32_MAX;
    rgbColor color = BACKGROUND;
    for (u8 i = 0; i < N_PLANES; ++i) {
        plane p = PLANES[i];
        f32   denom = dot(p.normal, ray_direction);
        if (denom < -EPSILON) {
            f32 t = (-p.delta - dot(p.normal, ray_origin)) / denom;
            if (t < threshold) {
                threshold = t;
                color = p.color;
            }
        }
    }
    for (u8 i = 0; i < N_SPHERES; ++i) {
        sphere s = SPHERES[i];
        vec3   sphere_origin = sub_vec3(ray_origin, s.center);
        f32    a = dot(ray_direction, ray_direction);
        f32    b = 2.0f * dot(sphere_origin, ray_direction);
        f32    c = dot(sphere_origin, sphere_origin) - (s.radius * s.radius);
        f32    denom = 2.0f * a;
        f32    root = sqrtf((b * b) - (4.0f * a * c));
        if (EPSILON < root) {
            f32 t_pos = (-b + root) / denom;
            f32 t_neg = (-b - root) / denom;
            f32 t = ((0.0f < t_neg) && (t_neg < t_pos)) ? t_neg : t_pos;
            if ((0.0f < t) & (t < threshold)) {
                threshold = t;
                color = s.color;
            }
        }
    }
    return color;
}

static void render(pixel* pixels) {
    vec3 camera_pos = {.x = 0.0f, .y = 10.0f, .z = 1.0f};
    vec3 camera_z = normalize_vec3(camera_pos);
    vec3 camera_x =
        normalize_vec3(cross_vec3(new_vec3(0.0f, 0.0f, 1.0f), camera_z));
    vec3 camera_y = normalize_vec3(cross_vec3(camera_z, camera_x));
    f32  film_dist = 1.0f;
    f32  film_width = 1.0f;
    f32  film_height = 1.0f;
    f32  film_half_width = 0.5f * film_width;
    f32  film_half_height = 0.5f * film_height;
    vec3 film_center = sub_vec3(camera_pos, mul_vec3_f32(camera_z, film_dist));
    for (u32 y = 0; y < HEIGHT; ++y) {
        f32 film_y = (2.0f * ((f32)y / HEIGHT_FLOAT)) - 1.0f;
        for (u32 x = 0; x < WIDTH; ++x) {
            f32  film_x = (2.0f * ((f32)x / WIDTH_FLOAT)) - 1.0f;
            vec3 film_pos = add_vec3(
                add_vec3(film_center,
                         mul_vec3_f32(camera_x, film_x * film_half_width)),
                mul_vec3_f32(camera_y, film_y * film_half_height));
            rgbColor color =
                cast_ray(camera_pos,
                         normalize_vec3(sub_vec3(film_pos, camera_pos)));
            pixels->red = color.red;
            pixels->green = color.green;
            pixels->blue = color.blue;
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
