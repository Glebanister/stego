#pragma once

static char *ERROR_MESSAGE[] = {
    "sussess!",
    "can't read file",
    "can't write to file",
    "wrong input format. Try './hw_01 help' to get help",
    "fragment doesn't fit to the picture",
    "no such flag",
    "unable to allocate memory",
    "unable to insert message",
    "unable to extract message"
};

typedef enum ERRORS {
    NO_ERROR,
    ERROR_FILE_READING,
    ERROR_FILE_WRITING,
    ERROR_WRONG_INPUT,
    ERROR_SIZE,
    ERROR_FLAG_EXIST,
    ERROR_OUT_OF_MEMORY,
    ERROR_INSERT,
    ERROR_EXTRACT
} ERRORS;
