#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <ctype.h>
#include <process.h>
#include "raylib.h"
#include "list.h"
#include "levenshtein.h"
#include "strmap.h"
#include "utils.h"
#include "stopwatch.h"

#define invalid_chars "\\/:*?\"<>|"
#define invalid_chars_len (array_len(invalid_chars) - 1)

LIST_DEF(String, char);
LIST_DEF(StringList, String);
LIST_DEF(CstrList, char*);

#define STR_ARENA_SIZE 1000
char STR_ARENA[STR_ARENA_SIZE];

FORCE_INLINE char* str_to_charptr(String *str) 
{
    if (str->count > STR_ARENA_SIZE) return NULL;

    char *ptr = &STR_ARENA[0];

    for (size_t i = 0; i < str->count; i++) {
        ptr[i] = str->items[i];
    }

    ptr[str->count] = '\0';


    return ptr;
}

void create_window(int width, int height, const char *title)
{
    InitWindow(width, height, title);
    ToggleBorderlessWindowed();
    SetWindowSize(width, height);
    // SetWindowPosition((1920 / 2) - (width / 2), (1080 / 2) - (75 / 2));
    SetWindowPosition((1920 / 2) - (width / 2), (1080 / 2) - (height / 2));
}

FORCE_INLINE void str_lower(String *s)
{
    for (size_t i = 0; i < s->count; i++) {
        s->items[i] = char_lower(s->items[i]);
    }
}

String *BUFF;

int compare_lev(const void *_x, const void *_y)
{
    int lx = strlen(*(char**)_x);
    int ly = strlen(*(char**)_y);
    
    String x = {
        .items = *(char**)_x,
        .count = lx,
        .capacity = lx,
    };
    String y = {
        .items = *(char**)_y,
        .count = ly,
        .capacity = ly,
    };
    int dx = levenshteine_distance(x.items, x.count, BUFF->items, BUFF->count);
    int dy = levenshteine_distance(y.items, y.count, BUFF->items, BUFF->count);

    return dx - dy;
}

FORCE_INLINE void delete_char(String *buffer, CstrList *bins)
{
    if (buffer->count <= 0) return;

    Unused(list_pop(buffer, char));
    qsort(bins->items, bins->count, sizeof(*bins->items), compare_lev);
}

FORCE_INLINE void delete_word(String *buffer, CstrList *bins)
{
    if (buffer->count <= 0) return;

    if (isspace(buffer->items[buffer->count - 1])) {
        while (isspace(buffer->items[buffer->count - 1]) && buffer->count > 0) {
            Unused(list_pop(buffer, char));
        }
    } 
    else {
        while (! isspace(buffer->items[buffer->count - 1]) && buffer->count > 0) {
            Unused(list_pop(buffer, char));
        }
    }

    qsort(bins->items, bins->count, sizeof(*bins->items), compare_lev);
}

FORCE_INLINE void add_bins(char *path, CstrList *bins)
{
    if (! DirectoryExists(path)) return;

    FilePathList flist = LoadDirectoryFilesEx(path, ".exe", false);        

    for (size_t j = 0; j < flist.count; j++) {
        char *cur_path = flist.paths[j];
        int start = -1;
        int end = -1;

        for (int k = strlen(cur_path) - 1; k >= 0; k--) {
            if (cur_path[k] == '.' && end == -1) {
                end = k;
            }
            else if ((cur_path[k] == '\\' || cur_path[k] == '/') && start == -1) {
                start = k + 1;
            }
            if (start != -1 && end != -1) {
                break;
            }
        }

        cur_path[end] = '\0';

        list_push(bins, &cur_path[start]);
    }
}

void* get_bins(void *_list)
{
    CstrList *bins = (CstrList*)_list;

    char *Path = getenv("path");

	char *path = Path; 

    for (size_t i = 0; Path[i] != '\0'; i++) {
        if (Path[i] != ';') continue;

        Path[i] = '\0';
    
        add_bins(path, bins);

        path = &Path[i + 1];
    }

    add_bins(path, bins);

    return NULL;
}

bool read_key_value(char* *key, char* *value, FILE *f)
{
    int i = 0;
    char buffer[100] = {0};

    while (!feof(f) && i < 100) {
        buffer[i] = getc(f);

        if (isspace(buffer[i])) continue;

        if (buffer[i] == ':') {
            int size1 = i + 1;
            *key = (char*)malloc(sizeof(char) * size1);             
            (*key)[size1 - 1] = '\0';
            buffer[i] = '\0';
            // printf("buffer1: %s\n", buffer);
            strcpy(*key, &buffer[0]);
            i++;
            int start = i;
            while (i < 100) {
                buffer[i] = getc(f);

                if (buffer[i] == ' ') continue;

                if (buffer[i] == '\n' || feof(f)) {
                    int size2 = (i - start) + 1;
                    *value = (char*)malloc(sizeof(char) * size2);
                    (*value)[size2 - 1] = '\0';
                    buffer[i] = '\0';
                    // printf("buffer2: %s\n", &buffer[start]);
                    strcpy(*value, &buffer[start]);
                
                    return true;
                }
                i++;
            }
        }

        i++;
    }

    return false;
}

StrMap import_aliases()
{
    const char *home_env = getenv("HOME");
    char aliases_path[100]; 
    sprintf(aliases_path, "%s%s", home_env, "\\.aliases.txt");
    StrMap map = strmap_new();

    if (! FileExists(aliases_path)) return map;

    FILE *f = fopen(aliases_path, "r");
    char *key;
    char *value;

    while (read_key_value(&key, &value, f)) { 
        strmap_add(&map, key, value);
    }

    fclose(f);

    return map;
}

void refresh_bins(CstrList *bins, char* *selected)
{
    qsort(bins->items, bins->count, sizeof(*bins->items), compare_lev);
    *selected = bins->items[0];
}

int main(void)
{
    SetTraceLogLevel(LOG_ERROR); 
    // SetExitKey(KEY_NULL);
    pthread_t thread;
    CstrList bins;
    list_allocs(&bins, 1000);
    pthread_create(&thread, NULL, get_bins, &bins);
    StrMap aliases = import_aliases();
    /* strmap_print(&aliases); */
    int height = 300;
    int width  = 300;
    String buffer = {0};
    list_alloc(&buffer);
    BUFF = &buffer;
    StopWatch backspace_sw = {0};
    StopWatch backspace_down_sw = {0};
    uint64_t backspace_speed_ms = 100;
    bool do_backspace = true;
    sw_start(&backspace_sw);
    sw_start(&backspace_down_sw);
    char *selected = "";

    create_window(width, height, "");

    while (! WindowShouldClose()) {
        char c;
        if ((c = GetCharPressed()) != 0) {
            if (!str_contains(invalid_chars, invalid_chars_len, c)) {
                list_push(&buffer, c);
                refresh_bins(&bins, &selected);
            } 
        }

        char *value = strmap_get(&aliases, str_to_charptr(&buffer));
        if (value != NULL) {
            selected = value;
        }

        switch (GetKeyPressed()) {
            case KEY_ENTER: {
                if (buffer.count > 0) {
                    static char temp[256];
                    sprintf(temp, "start /b %s", selected);
                    system(temp);
                    // _execlp(bins.items[0], bins.items[0], NULL);
                    list_clear(&buffer);
                }
                goto PROGRAM_END;
            }
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                delete_word(&buffer, &bins);
                do_backspace = true;
                sw_start(&backspace_sw);
                refresh_bins(&bins, &selected);
            }
        }
        else {
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (do_backspace) {
                    sw_start(&backspace_down_sw);
                    delete_char(&buffer, &bins);
                    do_backspace = false;
                    sw_start(&backspace_sw);
                    refresh_bins(&bins, &selected);
                } 
                else {
                    if (sw_elapsedms(&backspace_sw) > backspace_speed_ms && sw_elapsedms(&backspace_down_sw) > 300) {
                        delete_char(&buffer, &bins);
                        sw_start(&backspace_sw);
                        refresh_bins(&bins, &selected);
                    }
                }
            }
            if (IsKeyUp(KEY_BACKSPACE)) {
                do_backspace = true;
            }
        }

        BeginDrawing();
        ClearBackground(GRAY);
        {
            int size = 30;
            DrawRectangle(15, 15, width - 30, 45, DARKGRAY);
            DrawRectangle(15, 65, width - 30, 35, GetColor(0x181818FF));
            char *c_buffer = str_to_charptr(&buffer);
            int j = MeasureText(c_buffer, size);
            DrawRectangle(20 + j, 15, 5, 45, LIGHTGRAY);
            int start = 70;
            if (buffer.count > 0) {
                DrawText(c_buffer, 20, 25, size, WHITE);
                DrawText(selected, 20, 67, size, WHITE);
                for (int i = 1; i <= 6; i++) {
                    DrawText(bins.items[i - 1], 20, start + (33 * i), size, WHITE);
                }
            }
        }
        EndDrawing();
    }

PROGRAM_END:
    // pthread_join(thread, NULL);
    CloseWindow();
}
