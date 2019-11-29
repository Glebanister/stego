#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "stego.h"
#include "bmp.h"
#include "errors.h"

int handle_extract(int argc, char **argv)
{
    if (argc < 5)
    {
        return ERROR_WRONG_INPUT;
    }
    char *in_bmp = argv[2];
    char *key_file = argv[3];
    char *message_file = argv[4];
    image_s *picture = load_bmp(in_bmp);
    if (!picture)
    {
        return ERROR_FILE_READING;
    }
    cell_s *key = malloc(picture->w * picture->h * sizeof(cell_s));
    if (!key)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    size_t key_len = read_key(key_file, key, 0);
    invert_coords(key, key_len, picture->h);
    if (key_len == 0)
    {
        return ERROR_FILE_READING;
    }
    assert(key_len % BITS_PER_LETTER == 0);
    size_t message_len = key_len / BITS_PER_LETTER;
    fill_cells_with_file(key, key_len, picture);
    char *result = malloc(sizeof(char) * (message_len + 1));
    if (!result)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    extract_message(key, message_len, result);
    result[message_len] = '\0';
    int error;
    if (error = write_message(message_file, result))
    {
        destroy_image(picture);
        free(key);
        free(result);
        return error;
    }
    destroy_image(picture);
    free(key);
    free(result);
    return 0;
}

int handle_insert(int argc, char **argv)
{
    if (argc < 6)
    {
        return ERROR_WRONG_INPUT;
    }
    char *in_bmp = argv[2];
    char *out_bmp = argv[3];
    char *key_file = argv[4];
    char *message_file = argv[5];
    char *message = read_message(message_file);
    if (!message)
    {
        return ERROR_FILE_READING;
    }
    size_t message_len = strlen(message);
    size_t key_len = message_len * BITS_PER_LETTER;
    image_s *picture_in = load_bmp(in_bmp);
    if (!picture_in)
    {
        return ERROR_FILE_READING;
    }
    cell_s *key = malloc(sizeof(cell_s) * key_len);
    generate_key(key, message_len, picture_in->w, picture_in->h);
    invert_coords(key, key_len, picture_in->h);
    fill_cells_with_message(key, message);
    insert_message(key, message_len, picture_in);
    int error;
    if (error = save_bmp(out_bmp, picture_in))
    {
        free(message);
        free(key);
        destroy_image(picture_in);
        return error;
    }
    write_key(key, key_len);
    free(message);
    free(key);
    destroy_image(picture_in);
    return 0;
}

char *read_message(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t fsize = ftell(file);
    rewind(file);
    char *fcontent = (char *)malloc(sizeof(char) * (fsize + 1));
    if (fread(fcontent, 1, fsize, file) < fsize)
    {
        fclose(file);
        return NULL;
    }
    fclose(file);
    fcontent[fsize] = 0;
    return fcontent;
}

int write_message(char *filename, char *message)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        return ERROR_FILE_WRITING;
    }
    if (fprintf(file, "%s", message) < 0)
    {
        fclose(file);
        return ERROR_FILE_WRITING;
    }
    fclose(file);
    return 0;
}

char decode_char(size_t encoded)
{
    return (char)encoded;
}

size_t encode_char(char decoded)
{
    return (size_t)decoded;
}

void invert_coords(cell_s *key, size_t key_len, size_t height)
{
    for (size_t i = 0; i < key_len; i++)
    {
        key[i].y = height - key[i].y - 1;
    }
}

size_t read_key(char *filename, cell_s *key, size_t len)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        return 0;
    }
    size_t i = 0;
    for (; len == 0 || i < len; i++)
    {
        size_t x, y;
        char color;
        if (fscanf(file, "%ld %ld %c", &x, &y, &color) < 3)
        {
            break;
        }
        if (color == 'b')
        {
            key[i].color = 0;
        }
        else if (color == 'g')
        {
            key[i].color = 1;
        }
        else
        {
            key[i].color = 2;
        }
        key[i].x = x;
        key[i].y = y;
    }
    fclose(file);
    return i;
}

int fill_cells_with_message(cell_s *key, char *message)
{
    size_t len = strlen(message);
    for (size_t i = 0; i < len; i++)
    {
        size_t encoded_letter = encode_char(message[i]);
        for (size_t bit = 0; bit < BITS_PER_LETTER; bit++)
        {
            key[i * BITS_PER_LETTER + bit].bit = encoded_letter & (1 << bit) ? 1 : 0;
        }
    }
    return 0;
}

int fill_cells_with_file(cell_s *key, size_t key_len, image_s *image)
{
    for (size_t i = 0; i < key_len; i++)
    {
        key[i].bit = image->content[key[i].y * image->w * 3 + key[i].x * 3 + key[i].color] & 1;
    }
    return 0;
}

int extract_message(cell_s *encoded, size_t message_len, char *dest)
{
    for (size_t i = 0; i < message_len; i++)
    {
        size_t encoded_letter = 0;
        for (size_t bit = 0; bit < BITS_PER_LETTER; bit++)
        {
            encoded_letter |= encoded[i * BITS_PER_LETTER + bit].bit << bit;
        }
        char decoded_letter = decode_char(encoded_letter);
        dest[i] = decoded_letter;
    }
    return 0;
}

int insert_message(cell_s *decoded, size_t message_len, image_s *image)
{
    for (size_t i = 0; i < message_len; i++)
    {
        for (size_t bit = 0; bit < BITS_PER_LETTER; bit++)
        {
            size_t content_ind = decoded[i * BITS_PER_LETTER + bit].y * image->w * 3
                                 + decoded[i * BITS_PER_LETTER + bit].x * 3
                                 + decoded[i * BITS_PER_LETTER + bit].color;
            if (image->content[content_ind] & 1)
            {
                image->content[content_ind]--;
            }
            image->content[content_ind] |= decoded[i * BITS_PER_LETTER + bit].bit;
        }
    }
    return 0;
}


int generate_key(cell_s *key, size_t message_len, size_t width, size_t height)
{
    assert(key);
    for (size_t i = 0; i < message_len * BITS_PER_LETTER; i++)
    {
        key[i].color = rand() % 3;
        key[i].x = rand() % width;
        key[i].y = rand() % height;
    }
    return 0;
}

int write_key(cell_s *key, size_t key_len, char *key_file)
{
    
}