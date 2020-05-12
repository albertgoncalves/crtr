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
} BmpHeader;

typedef struct {
    u32 header_size;
    i32 pixel_width;
    i32 pixel_height;
    u16 color_planes;
    u16 bits_per_pixel;
    u8  _[24];
} DibHeader;

typedef struct {
    u8 blue;
    u8 green;
    u8 red;
} Pixel;

#define BMP_HEADER_SIZE sizeof(BmpHeader) + sizeof(DibHeader)
#define BMP_FILE_SIZE   BMP_HEADER_SIZE + sizeof(Pixel[SIZE])

#pragma pack(pop)

typedef struct {
    Pixel     pixels[SIZE];
    DibHeader dib_header;
    BmpHeader bmp_header;
} BmpBuffer;

static void set_bmp_header(BmpHeader* header) {
    header->id = 0x4d42;
    header->file_size = BMP_FILE_SIZE;
    header->header_offset = BMP_HEADER_SIZE;
}

static void set_dib_header(DibHeader* header) {
    header->header_size = sizeof(DibHeader);
    header->pixel_width = (i32)WIDTH;
    header->pixel_height = (i32)HEIGHT;
    header->color_planes = 1;
    header->bits_per_pixel = sizeof(Pixel) * 8;
}

static void write_bmp(FileHandle* file, BmpBuffer* buffer) {
    if (fwrite(&buffer->bmp_header, 1, sizeof(BmpHeader), file) !=
        sizeof(BmpHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->dib_header, 1, sizeof(DibHeader), file) !=
        sizeof(DibHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->pixels, 1, sizeof(Pixel[SIZE]), file) !=
        sizeof(Pixel[SIZE]))
    {
        exit(EXIT_FAILURE);
    }
}

#endif
