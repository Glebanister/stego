#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "bmp.h"
#include "stego.h"
#include "errors.h"

const char CMD_CROP_ROTATE[] = "crop-rotate";
const char CMD_INSERT[] = "insert";
const char CMD_EXTRACT[] = "extract";
const char CMD_HELP[] = "help";

const char TEXT_HELP[] = "Crop and rotate BMP images, insert stenography into images.\n"
                         "\n"
                         "Crop and rotate:\n"
                         "  './hw_01 crop-rotate ‹in-bmp› ‹out-bmp› ‹x› ‹y› ‹w› ‹h›'\n"
                         "  Parameters:\n"
                         "    ‹in-bmp› - name of the input file\n"
                         "    <out-bmp> - name of the output file\n"
                         "    <x>, <y> - coordinates of the upper left corner of the"
                         " area to be cut and rotated.\n"
                         "    <w>, <h> - width and height of the area before turning\n"
                         "\n"
                         "  Insert stenography:\n"
                         "  './hw_01 insert ‹in-bmp› ‹out-bmp› ‹key-txt› ‹msg-txt›'\n"
                         "  Extract stenography:\n"
                         "  './hw_01 extract ‹in-bmp› ‹key-txt› ‹msg-txt›'\n"
                         "  Parameters:\n"
                         "    <in-bmp> - name of the input file\n"
                         "    <out-bmp> - name of the output file\n"
                         "    <key-txt> - text file with key\n"
                         "    <msg-txt> - text file with message\n"
                         "\n"
                         "Encoding method.\n"
                         "The original message consists only of capital Latin letters, a space,"
                         " a period, and a comma. Each character is converted to a number from 0 to 28,"
                         " respectively (a total of 29 different values), and the number is converted "
                         "into five bits, written from the lowest to the highest. In total, a message of "
                         "N characters is encoded using 7N Bits.\n\n"
                         "To transmit a message, in addition to the carrier image, a key is required - "
                         "a text file that describes in which pixels the message bits are encoded. "
                         "In this file on separate lines are written:\n\n"
                         "  - The x and y coordinates (0 <= x < W, 0 <= y < H) of the pixel to which"
                         " the corresponding bit should be saved."
                         "  - The letter R / G / B denotes the color channel in the low bit of which "
                         "you want to write a message bit."
                         "If the key writes more bits than the message needs, the last lines are ignored.\n";

void print_error(int error_code)
{
    printf("Error: %s\n", ERROR_MESSAGE[error_code]);
}

int handle_crop_rotate(int argc, char **argv)
{
    if (argc < 8)
    {
        return ERROR_WRONG_INPUT;
    }
    char *in_bmp = argv[2];
    char *out_bmp = argv[3];
    size_t x = atoi(argv[4]);
    size_t y = atoi(argv[5]);
    size_t w = atoi(argv[6]);
    size_t h = atoi(argv[7]);
    image_s *picture = load_bmp(in_bmp);
    if (!picture)
    {
        return ERROR_FILE_READING;
    }
    bool fragment_fits = x + w <= picture->w && y + h <= picture->h;
    if (!fragment_fits)
    {
        destroy_image(picture);
        return ERROR_SIZE;
    }
    if (!picture)
    {
        destroy_image(picture);
        return ERROR_FILE_READING;
    }
    image_s *fragment = init_image(w, h, picture->header);
    if (!fragment)
    {
        return ERROR_OUT_OF_MEMORY;
    }
    crop(x, y, fragment, picture);
    int error = rotate(fragment);
    if (error)
    {
        return error;
    }
    if (error = save_bmp(out_bmp, fragment))
    {
        destroy_image(picture);
        destroy_image(fragment);
        return error;
    }
    destroy_image(picture);
    destroy_image(fragment);
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        print_error(ERROR_WRONG_INPUT);
        return ERROR_WRONG_INPUT;
    }
    char *flag = argv[1];
    int error;
    if (!strcmp(flag, CMD_HELP))
    {
        printf("%s\n", TEXT_HELP);
        return 0;
    }
    if (!strcmp(flag, CMD_CROP_ROTATE))
    {
        error = handle_crop_rotate(argc, argv);
    }
    else if (!strcmp(flag, CMD_EXTRACT))
    {
        error = handle_extract(argc, argv);
    }
    else if (!strcmp(flag, CMD_INSERT))
    {
        error = handle_insert(argc, argv);
    }
    else
    {
        error = ERROR_FLAG_EXIST;
    }
    if (error)
    {
        print_error(error);
    }
    return 0;
}
