#include "utils.h"
#include "levenshtein.h"

int partition(CstrList *arr1, String *arr2, int low, int high)
{
    int pivot = levenshteine_distance(arr1->items[high], strlen(arr1->items[high]), arr2->items, arr2->count);
    
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (levenshteine_distance(arr1->items[j], strlen(arr1->items[j]), arr2->items, arr2->count) < pivot) {
            i++;
            swap(arr1->items[i], arr1->items[j], char*);
        }
    }
    
    swap(arr1->items[i + 1], arr1->items[high], char*);  
    return i + 1;
}

void quickSort(CstrList *list, String *buffer, int low, int high) 
{
    if (low < high) {
        
        int pi = partition(list, buffer, low, high);

        quickSort(list, buffer, low, pi - 1);
        quickSort(list, buffer, pi + 1, high);
    }
}

bool str_contains(char* str, size_t size, char c)
{
    for (size_t i = 0; i < size; i++) {
        if (str[i] == c) return true;
    }

    return false;
}

void str_copy(char *dest, size_t dest_size, char *src, size_t src_size)
{
    if (dest_size < src_size) return;

    for (size_t i = 0; i < src_size && i < dest_size; i++) {
        dest[i] = src[i];
    }
}
