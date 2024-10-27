#include "strmap.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

uint32_t hash(char *str)
{
    uint32_t hash = 5381;
    int c = 1;

    while (c != 0) {
        c = *str;
        hash = ((hash << 5) + hash) + c;
        str++;
    }

    return hash;
}

StrMap strmap_new()
{
    StrMap map;
    map.capacity = DEFAULT_STRMAP_CAP;
    map.count = 0;
    map.items = (StrMap_Node*)calloc(map.capacity, sizeof(StrMap_Node));

    return map;
}

uint32_t strmap_get_hash(StrMap *map, char *key)
{
    return hash(key) % map->capacity;
}

uint32_t strmap_get_i(StrMap *map, char *key)
{
    uint32_t hash_value = strmap_get_hash(map, key);

    for (uint32_t i = 0; i < map->capacity; i++) {
        uint32_t index = (hash_value + i) % map->capacity;

        if (map->items[index].valid && strcmp(key, map->items[index].key) == 0) {
            return index;
        }
    }

    return 0;
}

bool strmap_add(StrMap *map, char *key, char* value)
{
    uint32_t hash_value = strmap_get_hash(map, key);
    
    for (uint32_t i = 0; i < map->capacity; i++) {
        uint32_t index = (hash_value + i) % map->capacity;

        if (! map->items[index].valid) {
            map->items[index].key = key;
            map->items[index].value = value;
            map->items[index].valid = true;
            map->count++;

            return true;
        }
    }
    
    return false;
}

char* strmap_get(StrMap *map, char *key)
{
    uint32_t hash_value = strmap_get_hash(map, key);
    
    for (uint32_t i = 0; i < map->capacity; i++) {
        uint32_t index = (hash_value + i) % map->capacity;

        if (map->items[index].valid && strcmp(key, map->items[index].key) == 0) {
            return map->items[index].value;
        }
    }
    
    return NULL;
}

bool strmap_has(StrMap *map, char *key)
{
    uint32_t hash_value = strmap_get_hash(map, key);
    
    for (uint32_t i = 0; i < map->capacity; i++) {
        uint32_t index = (hash_value + i) % map->capacity;

        if (map->items[index].valid && strcmp(key, map->items[index].key) == 0) {
            return true;
        }
    }
    
    return false;
}

void strmap_print(StrMap *map) 
{
    printf("{\n");
    for (uint32_t i = 0; i < map->capacity; i++) {
        if (map->items[i].valid) {
            printf("  %s : %s\n", map->items[i].key, map->items[i].value);
        }
    }
    printf("}\n");
}
