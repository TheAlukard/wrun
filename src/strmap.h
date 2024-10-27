#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *key;
    char* value;
    bool valid;
} StrMap_Node;

typedef struct {
    StrMap_Node *items;
    size_t count;
    size_t capacity;
} StrMap;

#define DEFAULT_STRMAP_CAP 150

StrMap strmap_new(void);
bool strmap_add(StrMap *map, char *key, char* value);
char* strmap_get(StrMap *map, char *key);
uint32_t strmap_get_hash(StrMap *map, char *key);
uint32_t strmap_get_i(StrMap *map, char *key);
bool strmap_has(StrMap *map, char *key);
void strmap_print(StrMap *map);
