#pragma once

#include <stdlib.h>
#include "bmp.h"

typedef struct cell_s
{
    size_t x, y, color;
    byte_t bit;
} cell_s;

int handle_extract(int argc, char **argv);

int handle_insert(int argc, char **argv);

char *read_message(char *filename);

int write_message(char *filename, char *message);

char decode_char(size_t encoded);

size_t encode_char(char decoded);

void invert_coords(cell_s *key, size_t key_len, size_t height);

size_t read_key(char *filename, cell_s *key, size_t len);

int fill_cells_with_message(cell_s *key, char *message);

int fill_cells_with_file(cell_s *key, size_t key_len, image_s *image);

int extract_message(cell_s *encoded, size_t message_len, char *dest);

int insert_message(cell_s *decoded, size_t message_len, image_s *image);

int generate_key(char *key_file, size_t message_len);