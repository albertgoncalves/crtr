#ifndef __BMP_H__
#define __BMP_H__

#include "color.h"
#include "types.h"

#pragma pack(push, 1)

typedef struct {
    u16 id;
    u32 file_size;
    u32 _;
    u32 header_offset;
} bmpHeader;

typedef struct {
    u32 header_size;
    u32 pixel_width;
    u32 pixel_height;
    u16 color_planes;
    u16 bits_per_pixel;
    u8  _[24];
} dibHeader;

typedef struct {
    u8 blue;
    u8 green;
    u8 red;
    u8 _;
} pixel;

#define BMP_HEADER_SIZE sizeof(bmpHeader) + sizeof(dibHeader)
#define BMP_FILE_SIZE   BMP_HEADER_SIZE + sizeof(pixel[SIZE])

#pragma pack(pop)

typedef struct {
    pixel     pixels[SIZE];
    dibHeader dib_header;
    bmpHeader bmp_header;
} bmpBuffer;

static void set_bmp_header(bmpHeader* header) {
    header->id = 0x4d42;
    header->file_size = BMP_FILE_SIZE;
    header->header_offset = BMP_HEADER_SIZE;
}

static void set_dib_header(dibHeader* header) {
    header->header_size = sizeof(dibHeader);
    header->pixel_width = WIDTH;
    header->pixel_height = HEIGHT;
    header->color_planes = 1;
    header->bits_per_pixel = sizeof(u32) * 8;
}

static void write_bmp(fileHandle* file, bmpBuffer* buffer) {
    if (fwrite(&buffer->bmp_header, 1, sizeof(bmpHeader), file) !=
        sizeof(bmpHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->dib_header, 1, sizeof(dibHeader), file) !=
        sizeof(dibHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->pixels, 1, sizeof(pixel[SIZE]), file) !=
        sizeof(pixel[SIZE]))
    {
        exit(EXIT_FAILURE);
    }
}

#endif