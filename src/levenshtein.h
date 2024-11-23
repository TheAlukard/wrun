#pragma once

#include <stdint.h>
#include "utils.h"

static int cache[1024][1024];

static inline int levenshteine_distance(char *s1, int len1, char *s2, int len2)
{
    for (int r = 0; r < len1 + 1; r++) {
        for (int c = 0; c < len2 + 1; c++) {
            cache[r][c] = INT32_MAX;
        }
    }

    for (int j = 0; j < len2 + 1; j++) 
        cache[len1][j] = len2 - j;
    for (int i = 0; i < len1 + 1; i++) 
        cache[i][len2] = len1 - i;

    for (int i = len1 - 1; i >= 0; i--) {
        for (int j = len2 - 1; j >= 0; j--) {
            if (to_lower(s1[i]) == to_lower(s2[j])) 
                cache[i][j] = cache[i + 1][j + 1];
            else 
                cache[i][j] = 1 + min(cache[i + 1][j], min(cache[i][j + 1], cache[i + 1][j + 1]));
        }
    }

    return cache[0][0]; 
}
