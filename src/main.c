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

#define WIDTH_FLOAT  1024.0f
#define HEIGHT_FLOAT 1152.0f

#define FILM_DISTANCE 1.0f
#define FILM_WIDTH    1.0f
#define FILM_HEIGHT   (1152.0f / 1024.0f)
#define EPSILON       0.001f

#define N_RAYS    2
#define N_PLANES  1
#define N_SPHERES 1

#define FILEPATH "out/main.bmp"

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

typedef struct {
    vec3 origin;
    vec3 direction;
} ray;

static vec3 CAMERA_POSITION = {.x = 0.0f, .y = 10.0f, .z = 1.0f};

static rgbColor BACKGROUND = {.red = 90, .green = 175, .blue = 235};

static plane PLANES[N_PLANES] = {
    {.normal = {.x = 0.0f, .y = 0.0f, .z = 1.0f},
     .delta = 1.0f,
     .color = {.red = 235, .green = 205, .blue = 190}},
};

static sphere SPHERES[N_SPHERES] = {
    {.center = {.x = 0.5f, .y = -2.0f, .z = 1.25f},
     .radius = 3.0f,
     .color = {.red = 85, .green = 240, .blue = 160}},
};

static rgbColor get_color(ray current_ray) {
    f32      threshold = F32_MAX;
    rgbColor color = BACKGROUND;
    u8       count = 0;
    ray      next_ray = {0};
    for (u8 _ = 0; _ < N_RAYS; ++_) {
        for (u8 i = 0; i < N_PLANES; ++i) {
            plane p = PLANES[i];
            f32   denom = dot(p.normal, current_ray.direction);
            if (denom < -EPSILON) {
                f32 t = (-p.delta - dot(p.normal, current_ray.origin)) / denom;
                if (t < threshold) {
                    threshold = t;
                    color = p.color;
                    next_ray.origin = mul_vec3_f32(current_ray.direction, t);
                    next_ray.direction = p.normal;
                    ++count;
                }
            }
        }
        for (u8 i = 0; i < N_SPHERES; ++i) {
            sphere s = SPHERES[i];
            vec3   sphere_origin = sub_vec3(current_ray.origin, s.center);
            f32    a = dot(current_ray.direction, current_ray.direction);
            f32    b = 2.0f * dot(sphere_origin, current_ray.direction);
            f32 c = dot(sphere_origin, sphere_origin) - (s.radius * s.radius);
            f32 denom = 2.0f * a;
            f32 root = sqrtf((b * b) - (4.0f * a * c));
            if (EPSILON < root) {
                f32 t_positive = (-b + root) / denom;
                f32 t_negative = (-b - root) / denom;
                f32 t = ((0.0f < t_negative) && (t_negative < t_positive))
                            ? t_negative
                            : t_positive;
                if ((0.0f < t) & (t < threshold)) {
                    threshold = t;
                    color = s.color;
                    next_ray.origin = mul_vec3_f32(current_ray.direction, t);
                    next_ray.direction =
                        normalize_vec3(sub_vec3(next_ray.origin, s.center));
                    ++count;
                }
            }
        }
        if (0 < count) {
            current_ray.origin = next_ray.origin;
            current_ray.direction = next_ray.direction;
        } else {
            break;
        }
    }
    return color;
}

static void render(pixel* pixels) {
    {
        /* NOTE: Hmm... is this necessary? */
        for (u8 i = 0; i < N_PLANES; ++i) {
            plane p = PLANES[i];
            p.normal = normalize_vec3(p.normal);
        }
    }
    vec3 camera_z = normalize_vec3(CAMERA_POSITION);
    vec3 camera_x =
        normalize_vec3(cross_vec3(new_vec3(0.0f, 0.0f, 1.0f), camera_z));
    vec3 camera_y = normalize_vec3(cross_vec3(camera_z, camera_x));
    vec3 film_center =
        sub_vec3(CAMERA_POSITION, mul_vec3_f32(camera_z, FILM_DISTANCE));
    f32 film_half_width = 0.5f * FILM_WIDTH;
    f32 film_half_height = 0.5f * FILM_HEIGHT;
    for (u32 y = 0; y < HEIGHT; ++y) {
        f32 film_y = (2.0f * ((f32)y / HEIGHT_FLOAT)) - 1.0f;
        for (u32 x = 0; x < WIDTH; ++x) {
            f32  film_x = (2.0f * ((f32)x / WIDTH_FLOAT)) - 1.0f;
            vec3 film_position = add_vec3(
                add_vec3(film_center,
                         mul_vec3_f32(camera_x, film_x * film_half_width)),
                mul_vec3_f32(camera_y, film_y * film_half_height));
            ray film_ray = {
                .origin = CAMERA_POSITION,
                .direction =
                    normalize_vec3(sub_vec3(film_position, CAMERA_POSITION)),
            };
            rgbColor color = get_color(film_ray);
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
