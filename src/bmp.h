#ifndef __BMP_H__
#define __BMP_H__

typedef FILE File;

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
    u8  _[24u];
} DibHeader;

typedef struct {
    u8 blue;
    u8 green;
    u8 red;
} Pixel;

#define BMP_HEADER_SIZE sizeof(BmpHeader) + sizeof(DibHeader)
#define BMP_FILE_SIZE   BMP_HEADER_SIZE + sizeof(Pixel[N_PIXELS])

#pragma pack(pop)

typedef struct {
    Pixel     pixels[N_PIXELS];
    DibHeader dib_header;
    BmpHeader bmp_header;
} BmpBuffer;

static void set_bmp_header(BmpHeader* header) {
    header->id = 0x4D42u;
    header->file_size = BMP_FILE_SIZE;
    header->header_offset = BMP_HEADER_SIZE;
}

static void set_dib_header(DibHeader* header) {
    header->header_size = sizeof(DibHeader);
    header->pixel_width = (i32)WIDTH;
    header->pixel_height = (i32)HEIGHT;
    header->color_planes = 1u;
    header->bits_per_pixel = sizeof(Pixel) * 8u;
}

static void write_bmp(File* file, BmpBuffer* buffer) {
    if (fwrite(&buffer->bmp_header, 1u, sizeof(BmpHeader), file) !=
        sizeof(BmpHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->dib_header, 1u, sizeof(DibHeader), file) !=
        sizeof(DibHeader))
    {
        exit(EXIT_FAILURE);
    }
    if (fwrite(&buffer->pixels, 1u, sizeof(Pixel[N_PIXELS]), file) !=
        sizeof(Pixel[N_PIXELS]))
    {
        exit(EXIT_FAILURE);
    }
}

#endif
