#include <stdio.h>
#include <stdlib.h>

#include "color.h"

/* NOTE: See
 * `https://www.gabrielgambetta.com/computer-graphics-from-scratch/`.
 */

#pragma pack(push, 1)

typedef struct {
    rgbColor color;
    u8       alpha;
} pixel;

#pragma pack(pop)

int main(void) {
    rgbColor a = {.red = 50, .green = 100, .blue = 150};
    rgbColor b = {.red = 0, .green = 50, .blue = 100};
    printf("a: %3hhu %3hhu %3hhu\n", a.red, a.green, a.blue);
    printf("b: %3hhu %3hhu %3hhu\n", b.red, b.green, b.blue);
    a = mul_color(a, 2);
    rgbColor c = add_color(a, b);
    printf("c: %3hhu %3hhu %3hhu\n", c.red, c.green, c.blue);
    b = add_color(b, b);
    c = sub_color(c, a);
    a = div_color(a, 3);
    printf("a: %3hhu %3hhu %3hhu\n", a.red, a.green, a.blue);
    printf("b: %3hhu %3hhu %3hhu\n", b.red, b.green, b.blue);
    printf("c: %3hhu %3hhu %3hhu\n", c.red, c.green, c.blue);
}
