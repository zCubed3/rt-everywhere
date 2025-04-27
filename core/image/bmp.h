//
// Copyright (c) 2023-2025 Liam R. (zCubed3)
//

#ifndef RTEVERYWHERE_BMP_H
#define RTEVERYWHERE_BMP_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct bmp_header_s {
    uint16_t ident; // Identifier of DIB data
    uint32_t size; // Size of BMP data
    uint16_t reserved0;
    uint16_t reversed1;
    uint32_t offset; // Offset to BMP data
} bmp_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct bmp_info_s {
    uint32_t size; // Size of this info block
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_per_m;
    uint32_t y_per_m;
    uint32_t color_usage;
    uint32_t importance;
} bmp_info_t;
#pragma pack(pop)

extern void write_bmp(const char* path, uint16_t width, uint16_t height, char* rgb);

#endif //RTEVERYWHERE_BMP_H
