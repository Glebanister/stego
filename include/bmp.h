#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define HEADER_SIZE 54
#define MEM_POS 2
#define WIDTH_POS 18
#define HEIGHT_POS 22
#define IMAGE_SIZE_POS 34
#define BITS_PER_LETTER 7

typedef uint8_t byte_t;

typedef struct image_s
{
    size_t w, h;
    byte_t *content;
    byte_t *header;
} image_s;

size_t calc_shift(size_t width);

image_s *init_image(size_t w, size_t h, byte_t *header);

void destroy_image(image_s *image);

image_s *load_bmp(char *filename);

int crop(size_t x, size_t y, image_s *fragment, image_s *picture);

int rotate(image_s *picture);

int save_bmp(char *filename, image_s *picture);
