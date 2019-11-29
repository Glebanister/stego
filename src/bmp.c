#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "bmp.h"
#include "errors.h"

image_s *init_image(size_t w, size_t h, byte_t *header)
{
    image_s *image = malloc(sizeof(image_s));
    if (!image)
    {
        return NULL;
    }
    image->w = w;
    image->h = h;
    image->content = malloc(sizeof(byte_t) * w * h * 3);
    image->header = NULL;
    if (header)
    {
        image->header = malloc(sizeof(byte_t) * HEADER_SIZE);
        if (!image->header)
        {
            return NULL;
        }
        memcpy(image->header, header, HEADER_SIZE);
    }
    return image;
}

void destroy_image(image_s *image)
{
    assert(image);
    free(image->content);
    free(image->header);
    free(image);
}

void bytes_to_int(size_t *value, byte_t *src, size_t start)
{
    assert(src);
    assert(value);
    *value = 0;
    for (size_t byte = 0; byte < 4; byte++)
    {
        *value |= (src[start + byte] << (8 * byte));
    }
}

void int_to_bytes(size_t value, byte_t *dest, size_t start)
{
    assert(dest);
    for (size_t byte = 0; byte < 4; byte++)
    {
        dest[start + byte] = (value >> (8 * byte));
    }
}

size_t calc_shift(size_t width)
{
    return width % 4 != 0 ? 4 - (3 * width) % 4 : 0;
}

image_s *load_bmp(char *filename)
{
    assert(filename);
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        return NULL;
    }
    image_s *result = malloc(sizeof(image_s));
    if (!result)
    {
        fclose(file);
        return NULL;
    }
    result->header = malloc(sizeof(byte_t) * HEADER_SIZE);
    if (!result->header)
    {
        fclose(file);
        return NULL;
    }
    if (fread(result->header, sizeof(byte_t), HEADER_SIZE, file) < HEADER_SIZE)
    {
        fclose(file);
        return NULL;
    }
    size_t width = 0, height = 0;
    bytes_to_int(&width, result->header, WIDTH_POS);
    bytes_to_int(&height, result->header, HEIGHT_POS);
    size_t size = 3 * width * height;
    result->w = width;
    result->h = height;
    result->content = malloc(sizeof(byte_t) * size);
    if (!result->content)
    {
        free(result->header);
        free(result);
        fclose(file);
        return NULL;
    }
    size_t shift = calc_shift(width);
    for (size_t j = 0; j < height; j++)
    {
        if (fread(result->content + j * width * 3, sizeof(byte_t) * 3, width, file) < width)
        {
            free(result->header);
            free(result);
            fclose(file);
            return NULL;
        }
        fseek(file, shift, SEEK_CUR);
    }
    fclose(file);
    return result;
}

int crop(size_t x, size_t y, image_s *fragment, image_s *picture)
{
    assert(fragment);
    assert(picture);
    for (size_t i = x; i < x + fragment->w; i++)
    {
        for (size_t j = picture->h - y - fragment->h; j < picture->h - y; j++)
        {
            memcpy(
                &fragment->content[3 * (i - x) + 3 * fragment->w * (j - (picture->h - y - fragment->h))],
                &picture->content[3 * i + 3 * picture->w * j],
                3);
        }
    }
    return 0;
}

int rotate(image_s *picture)
{
    assert(picture);
    byte_t *rotated = malloc(sizeof(byte_t) * 3 * picture->w * picture->h);
    if (!rotated)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    for (size_t j = 0; j < picture->h; j++)
    {
        for (size_t i = 0; i < picture->w; i++)
        {
            memcpy(
                &rotated[3 * j + (picture->w - i - 1) * 3 * picture->h],
                &picture->content[3 * i + j * 3 * picture->w],
                3);
        }
    }
    {
        size_t width = picture->w;
        picture->w = picture->h;
        picture->h = width;
    }
    memcpy(picture->content, rotated, picture->w * picture->h * 3);
    free(rotated);
    return 0;
}

int save_bmp(char *filename, image_s *picture)
{
    assert(filename);
    assert(picture);
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        return ERROR_FILE_WRITING;
    }
    size_t shift = calc_shift(picture->w);
    int_to_bytes(
        picture->w * picture->h * 3 + HEADER_SIZE + shift * picture->h,
        picture->header,
        MEM_POS);
    int_to_bytes(
        picture->w,
        picture->header,
        WIDTH_POS);
    int_to_bytes(
        picture->h,
        picture->header,
        HEIGHT_POS);
    int_to_bytes(
        picture->w * picture->h * 3 + shift * picture->h,
        picture->header,
        IMAGE_SIZE_POS);
    if (fwrite(picture->header, sizeof(byte_t), HEADER_SIZE, file) < HEADER_SIZE)
    {
        fclose(file);
        return ERROR_FILE_WRITING;
    }
    for (size_t j = 0; j < picture->h; j++)
    {
        if (fwrite(picture->content + j * picture->w * 3, sizeof(byte_t) * 3, picture->w, file) < picture->w)
        {
            fclose(file);
            return ERROR_FILE_WRITING;
        }
        byte_t trash = 0;
        for (size_t trash_i = 0; trash_i < shift; trash_i++)
        {
            if (fwrite(&trash, 1, 1, file) < 1)
            {
                fclose(file);
                return ERROR_FILE_WRITING;
            }
        }
    }
    fclose(file);
    return 0;
}
