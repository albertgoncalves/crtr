#include <stdio.h>
#include <stdlib.h>

/* NOTE: See `https://www.gabrielgambetta.com/computer-graphics-from-scratch/`.
 */

#define WIDTH  1024
#define HEIGHT 1024
#define SIZE   1048576

#define FILEPATH "out/main.png"

#include "bmp.h"

#define N 8
static void set_pixels(pixel* pixels) {
    rgbColor colors[N] = {
        {.red = 50, .green = 100, .blue = 150},
        {.red = 0, .green = 50, .blue = 100},
        {.red = 0, .green = 255, .blue = 255},
        {.red = 255, .green = 0, .blue = 255},
        {.red = 255, .green = 255, .blue = 0},
        {.red = 255, .green = 0, .blue = 0},
        {.red = 0, .green = 255, .blue = 0},
        {.red = 0, .green = 0, .blue = 255},
    };
    colors[0] = mul_color(colors[0], 4);
    colors[2] = add_color(colors[0], colors[1]);
    colors[1] = add_color(colors[1], colors[1]);
    colors[2] = sub_color(colors[2], colors[0]);
    colors[0] = div_color(colors[0], 2);
    for (u32 i = 0; i < SIZE; ++i) {
        rgbColor* color = &colors[i % N];
        pixels->red = color->red;
        pixels->green = color->green;
        pixels->blue = color->blue;
        ++pixels;
    }
}
#undef N

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
    set_pixels(buffer->pixels);
    write_bmp(file, buffer);
    fclose(file);
    free(buffer);
}
