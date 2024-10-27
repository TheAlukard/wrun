#pragma once

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
