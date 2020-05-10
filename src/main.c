#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* NOTE: See `https://www.gabrielgambetta.com/computer-graphics-from-scratch/`.
 * NOTE: See `https://www.youtube.com/watch?v=pq7dV4sR7lg`.
 */

#define WIDTH  512
#define HEIGHT 576
#define SIZE   294912

#define WIDTH_FLOAT       512.0f
#define HEIGHT_FLOAT      576.0f
#define HALF_WIDTH_FLOAT  256.0f
#define HALF_HEIGHT_FLOAT 288.0f

#define VIEWPORT_SIZE      1.5f
#define PROJECTION_PLANE_Z 1.0f
#define MIN_DISTANCE       1.0f
#define EPSILON            0.1f

#define N_SPHERES     4
#define N_LIGHTS      3
#define REFLECT_DEPTH 5

#define FILEPATH "out/main.bmp"

#include "bmp.h"

typedef struct {
    Vec3     center;
    RgbColor color;
    f32      radius;
    f32      specular;
    f32      reflective;
} Sphere;

typedef enum {
    AMBIENT = 0,
    POINT,
    DIRECTIONAL,
} LightType;

typedef struct {
    LightType type;
    f32       intensity;
    Vec3      position;
} Light;

typedef enum {
    FALSE = 0,
    TRUE,
} Bool;

typedef struct {
    f32  t;
    u8   index;
    Bool exists;
} Intersection;

typedef struct {
    RgbColor color;
    f32      reflective;
} Reflection;

static Vec3 CAMERA_POSITION = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
static Vec3 CAMERA_ROTATION[3] = {
    {.x = 1.0f, .y = 0.0f, .z = 0.0f},
    {.x = 0.0f, .y = 1.0f, .z = 0.0f},
    {.x = 0.0f, .y = 0.0f, .z = 1.0f},
};

static RgbColor BACKGROUND = {.red = 245, .green = 245, .blue = 245};

static Sphere SPHERES[N_SPHERES] = {
    {.center = {.x = 0.0f, .y = -0.75f, .z = 4.5f},
     .radius = 1.0f,
     .color = {.red = 85, .green = 240, .blue = 160},
     .specular = 500.0f,
     .reflective = 0.2f},
    {.center = {.x = -2.0f, .y = 0.5f, .z = 5.0f},
     .radius = 1.0f,
     .color = {.red = 240, .green = 160, .blue = 85},
     .specular = 500.0f,
     .reflective = 0.4f},
    {.center = {.x = 2.0f, .y = -0.5f, .z = 3.0f},
     .radius = 1.0f,
     .color = {.red = 160, .green = 85, .blue = 240},
     .specular = 10.f,
     .reflective = 0.25f},
    {.center = {.x = 0.0f, .y = -5001.0f, .z = 0.0f},
     .radius = 5000.0f,
     .color = {.red = 90, .green = 90, .blue = 90},
     .specular = 1000.0f,
     .reflective = 0.1f},
};

static Light LIGHTS[N_LIGHTS] = {
    {.type = AMBIENT, .intensity = 0.2f, .position = {0}},
    {.type = POINT,
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

static Intersection nearest_intersection(Vec3 origin,
                                         Vec3 direction,
                                         f32  min_distance,
                                         f32  max_distance) {
    Intersection result = {
        .t = F32_MAX,
        .index = 0,
        .exists = FALSE,
    };
    for (u8 i = 0; i < N_SPHERES; ++i) {
        Sphere sphere = SPHERES[i];
        Vec3   center = sub_vec3(origin, sphere.center);
        f32    k1 = dot_vec3(direction, direction);
        f32    k2 = 2.0f * dot_vec3(center, direction);
        f32    k3 = dot_vec3(center, center) - (sphere.radius * sphere.radius);
        f32    discriminant = (k2 * k2) - (4.0f * k1 * k3);
        if (EPSILON < discriminant) {
            f32 t1 = (-k2 - sqrtf(discriminant)) / (2.0f * k1);
            f32 t2 = (-k2 + sqrtf(discriminant)) / (2.0f * k1);
            if ((t1 < result.t) && (min_distance < t1) && (t1 < max_distance))
            {
                result.t = t1;
                result.index = i;
                result.exists = TRUE;
            }
            if ((t2 < result.t) && (min_distance < t2) && (t2 < max_distance))
            {
                result.t = t2;
                result.index = i;
                result.exists = TRUE;
            }
        }
    }
    return result;
}

static Vec3 reflect(Vec3 a, Vec3 b) {
    Vec3 result = sub_vec3(mul_vec3_f32(b, 2.0f * dot_vec3(a, b)), a);
    return result;
}

static f32 light_intensity(Vec3 point, Vec3 normal, Vec3 view, f32 specular) {
    f32 intensity = 0.0f;
    f32 len_normal = len_vec3(normal);
    f32 len_view = len_vec3(view);
    for (u8 i = 0; i < N_LIGHTS; ++i) {
        Light light = LIGHTS[i];
        if (light.type == AMBIENT) {
            intensity += light.intensity;
        } else {
            Vec3 position;
            f32  t;
            if (light.type == POINT) {
                position = sub_vec3(light.position, point);
                t = 1.0;
            } else {
                position = light.position;
                t = F32_MAX;
            }
            Intersection shadow =
                nearest_intersection(point, position, EPSILON, t);
            if (shadow.exists == TRUE) {
                continue;
            }
            f32 reflection_diffuse = dot_vec3(normal, position);
            if (0.0f < reflection_diffuse) {
                intensity += (light.intensity * reflection_diffuse) /
                             (len_normal * len_vec3(position));
            }
            Vec3 reflection_specular = reflect(normal, position);
            f32  reflection = dot_vec3(reflection_specular, view);
            if (0.0f < reflection) {
                intensity += light.intensity *
                             powf(reflection / (len_vec3(reflection_specular) *
                                                len_view),
                                  specular);
            }
        }
    }
    return intensity;
}

static void render(Pixel* pixels) {
    Vec3 camera_direction = {
        .x = 0.0f,
        .y = 0.0f,
        .z = PROJECTION_PLANE_Z,
    };
    Reflection reflections[REFLECT_DEPTH] = {0};
    f32        min_distance = MIN_DISTANCE;
    for (f32 y = -HALF_HEIGHT_FLOAT; y < HALF_HEIGHT_FLOAT; ++y) {
        camera_direction.y = (y * VIEWPORT_SIZE) / HEIGHT_FLOAT;
        for (f32 x = -HALF_WIDTH_FLOAT; x < HALF_WIDTH_FLOAT; ++x) {
            camera_direction.x = (x * VIEWPORT_SIZE) / WIDTH_FLOAT;
            Vec3 ray_position = CAMERA_POSITION;
            Vec3 ray_direction = {0};
            {
                ray_direction.x += CAMERA_ROTATION[0].x * camera_direction.x;
                ray_direction.x += CAMERA_ROTATION[0].y * camera_direction.y;
                ray_direction.x += CAMERA_ROTATION[0].z * camera_direction.z;
                ray_direction.y += CAMERA_ROTATION[1].x * camera_direction.x;
                ray_direction.y += CAMERA_ROTATION[1].y * camera_direction.y;
                ray_direction.y += CAMERA_ROTATION[1].z * camera_direction.z;
                ray_direction.z += CAMERA_ROTATION[2].x * camera_direction.x;
                ray_direction.z += CAMERA_ROTATION[2].y * camera_direction.y;
                ray_direction.z += CAMERA_ROTATION[2].z * camera_direction.z;
            }
            u8 index = 0;
            for (u8 i = 0; i < REFLECT_DEPTH; ++i) {
                Intersection intersection = nearest_intersection(ray_position,
                                                                 ray_direction,
                                                                 min_distance,
                                                                 F32_MAX);
                if (intersection.exists == TRUE) {
                    Sphere sphere = SPHERES[intersection.index];
                    Vec3   point =
                        add_vec3(ray_position,
                                 mul_vec3_f32(ray_direction, intersection.t));
                    Vec3 normal = sub_vec3(point, sphere.center);
                    normal = mul_vec3_f32(normal, 1.0f / len_vec3(normal));
                    Vec3 view = mul_vec3_f32(ray_direction, -1.0f);
                    f32  intensity =
                        light_intensity(point, normal, view, sphere.specular);
                    Reflection* reflection = &reflections[index++];
                    reflection->color.red =
                        mul_u8_f32(sphere.color.red, intensity);
                    reflection->color.blue =
                        mul_u8_f32(sphere.color.blue, intensity);
                    reflection->color.green =
                        mul_u8_f32(sphere.color.green, intensity);
                    reflection->reflective = sphere.reflective;
                    if (sphere.reflective <= 0.0f) {
                        break;
                    }
                    ray_position = point;
                    ray_direction = reflect(view, normal);
                    min_distance = EPSILON;
                } else {
                    reflections[index++].color = BACKGROUND;
                    break;
                }
            }
            if (0 < index) {
                for (u8 i = (u8)(index - 1); 0 < i; --i) {
                    u8       j = (u8)(i - 1);
                    RgbColor reflection = reflections[i].color;
                    RgbColor color = reflections[j].color;
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
                RgbColor color = reflections[0].color;
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
    FileHandle* file = fopen(FILEPATH, "wb");
    if (file == NULL) {
        return EXIT_FAILURE;
    }
    BmpBuffer* buffer = calloc(sizeof(BmpBuffer), 1);
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
