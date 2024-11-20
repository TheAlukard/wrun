#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "raylib.h"

#define Vec2(x_, y_) ((Vector2){.x = x_, .y = y_})

static inline void create_window(int width, int height, const char *title)
{
    InitWindow(width, height, title);
    ToggleBorderlessWindowed();
    SetWindowSize(width, height);
    // SetWindowPosition((1920 / 2) - (width / 2), (1080 / 2) - (75 / 2));
    SetWindowPosition((1920 / 2) - (width / 2), (1080 / 2) - (height / 2));
}

static inline void log_here(FILE *f, const char *str)
{
    fwrite(str, sizeof(char), strlen(str), f);
    fwrite("\n", sizeof(char), 1, f);
    fflush(f);
}


static inline bool str_contains(char* str, size_t size, char c)
{
    for (size_t i = 0; i < size; i++) {
        if (str[i] == c) return true;
    }

    return false;
}

static inline void str_copy(char *dest, size_t dest_size, char *src, size_t src_size)
{
    if (dest_size < src_size) return;

    for (size_t i = 0; i < src_size && i < dest_size; i++) {
        dest[i] = src[i];
    }
}

static inline char char_lower(char c) 
{
    if (c >= 65 && c <= 90) {
        return c + 32;
    }

    return c;
}
