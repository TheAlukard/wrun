#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "raylib.h"
#include "list.h"
#include "levenshtein.h"
#include "strmap.h"
#include "utils.h"
#include "bins.h"
#define SIMPLE_CALC_IMPLEMENTATION
#include "simple_calc.h"

#define invalid_chars "\\/:*?\"<>|"
#define invalid_chars_len (array_len(invalid_chars) - 1)
#define STR_ARENA_SIZE 1000
char STR_ARENA[STR_ARENA_SIZE];

LIST_DEF(String, char);
LIST_DEF(StringList, String);

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

void refresh_bins(Bins *bins, char* *selected);

FORCE_INLINE void delete_char(String *buffer, Bins *bins)
{
    if (buffer->count <= 0) return;

    Unused(list_pop(buffer, char));
    refresh_bins(bins, NULL);
}

FORCE_INLINE void delete_word(String *buffer, Bins *bins)
{
    if (buffer->count <= 0) return;

    if (isspace(buffer->items[buffer->count - 1])) {
        while (isspace(buffer->items[buffer->count - 1]) && buffer->count > 0) {
            Unused(list_pop(buffer, char));
        }
    } 
    else {
        while (!isspace(buffer->items[buffer->count - 1]) && buffer->count > 0) {
            Unused(list_pop(buffer, char));
        }
    }

    refresh_bins(bins, NULL);
}

FORCE_INLINE void add_bins(char *path, Bins *bins)
{
    if (!DirectoryExists(path)) return;

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

        list_push(get_strlist(bins, cur_path[start]), &cur_path[start]);
    }
}

void* get_bins(void *_list)
{
    Bins *bins = (Bins*)_list;

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

        if (isspace(buffer[i]) && buffer[i] != ' ') continue;

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

StrMap import_aliases(void)
{
    const char *home_env = getenv("HOME");
    char aliases_path[100]; 
    sprintf(aliases_path, "%s%s", home_env, "\\.aliases.txt");
    StrMap map = strmap_new();

    if (!FileExists(aliases_path)) return map;

    FILE *f = fopen(aliases_path, "r");
    char *key;
    char *value;

    while (read_key_value(&key, &value, f)) { 
        strmap_add(&map, key, value);
    }

    fclose(f);

    return map;
}

void refresh_bins(Bins *bins, char* *selected)
{
    if (BUFF->count <= 0) return;

    CstrList *list;
    if (str_contains(invalid_chars, invalid_chars_len, BUFF->items[0]) && BUFF->count > 1) {
        list = get_strlist(bins, BUFF->items[1]);
    }
    else {
        list = get_strlist(bins, BUFF->items[0]);
    }

    if (list->count <= 0) return;

    qsort(list->items, list->count, sizeof(*list->items), compare_lev);
    if (selected != NULL) *selected = list->items[0];
}

int main(void)
{
    SetTraceLogLevel(LOG_ERROR); 
    
    pthread_t thread;
    Bins *bins = bins_alloc();
    pthread_create(&thread, NULL, get_bins, bins);
    StrMap aliases = import_aliases();
    const int HEIGHT = 300;
    const int WIDTH  = 300;
    create_window(WIDTH, HEIGHT, "", 60);
    String buffer = {0};
    BUFF = &buffer;
    int backspace_frames = 0;
    char *selected = "";
    char calc_buffer[256];
    int font_size = 30;

    Font font = LoadFontEx("C:/windows/Fonts/CascadiaCode.ttf", font_size, NULL, 0);

    while (!WindowShouldClose()) {
        char c;
        if ((c = GetCharPressed()) != 0) {
            list_push(&buffer, c);
            refresh_bins(bins, &selected);
        }

        char *value = strmap_get(&aliases, str_to_charptr(&buffer));
        if (value != NULL) {
            selected = value;
        }

        if (buffer.count > 1 && buffer.items[0] == '=') {
            sprintf(calc_buffer, "%.*s", (int)buffer.count - 1, &buffer.items[1]);
            double result = sc_calculate(calc_buffer);
            // sprintf(calc_buffer, "%lf", result);
            print_fraction(calc_buffer, result);
            selected = calc_buffer;
        }

        switch (GetKeyPressed()) {
            case KEY_ENTER: {
                if (buffer.count <= 0) break;

                static char temp[256];
                if (buffer.items[0] == '/' && buffer.count > 1) {
                    sprintf(temp, "%.*s", (int)buffer.count - 1, &buffer.items[1]);
                    system(temp);
                }
                else if (buffer.items[0] == '=') {
                    SetClipboardText(selected);
                }
                else {
                    sprintf(temp, "start /b %s", selected);
                    system(temp);
                }

                list_clear(&buffer);

                goto PROGRAM_END;
            }
            case KEY_BACKSPACE: {
                delete_char(&buffer, bins);
                refresh_bins(bins, &selected);
                backspace_frames = 0;
                break;
            }
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed(KEY_BACKSPACE)) {
                delete_word(&buffer, bins);
                refresh_bins(bins, &selected);
            }
            else if (IsKeyPressed(KEY_V)) {
                const char *clipboard = GetClipboardText();

                for (size_t i = 0; i < strlen(clipboard); i++) {
                    list_push(&buffer, clipboard[i]);
                }
                refresh_bins(bins, &selected);
            }
        }
        else {
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (backspace_frames > GetFPS() / 2) {
                    delete_char(&buffer, bins);
                    refresh_bins(bins, &selected);
                }
            }
        }

        backspace_frames++;

        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        {
            float spacing = 0.8;
            DrawRectangle(15, 15, WIDTH - 30, 45, DARKGRAY);
            DrawRectangle(15, 65, WIDTH - 30, 35, GRAY);
            char *c_buffer = str_to_charptr(&buffer);
            Vector2 j = MeasureTextEx(font, c_buffer, font_size, spacing);
            Vector2 k = MeasureTextEx(font, selected, font_size, spacing);
            int start = 70;
            int showed = 6;

            DrawRectangle(20 + j.x, 15, 5, 45, LIGHTGRAY);

            if (buffer.count > 0) {
                if (k.x > WIDTH - 35) {
                    float width_per_char = k.x / strlen(selected);
                    int last_char = floor((WIDTH - 35) / width_per_char);
                    int mag = ceil(k.x / (WIDTH - 35.f) + 0.1);
                    char temp[100];
                    for (int i = 1; i < mag; i++) {
                        DrawRectangle(15, 65 + (i * 35), WIDTH - 30, 35, GRAY);
                        start += 33;
                        showed--;
                        char *text = &selected[last_char * (i - 1)];
                        sprintf(temp, "%.*s", last_char, text);
                        DrawTextEx(font, temp, Vec2(20, 67 + ((i - 1) * 35)), font_size, spacing, WHITE);
                    }
                    char *text = &selected[last_char * (mag - 1)];
                    DrawTextEx(font, text, Vec2(20, 67 + ((mag - 1) * 35)), font_size, spacing, WHITE);
                }
                else {
                    DrawTextEx(font, selected, Vec2(20, 67), font_size, spacing, WHITE);
                }
                CstrList *bin_list = get_strlist(bins, selected[0]);
                DrawTextEx(font, c_buffer, Vec2(20, 25), font_size, spacing, WHITE);
                for (int i = 1; i < (int)bin_list->count && i <= showed; i++) {
                    DrawTextEx(font, bin_list->items[i - 1], Vec2(20, start + (33 * i)), font_size, spacing, WHITE);
                }
            }
        }
        EndDrawing();
    }

PROGRAM_END:
    /* pthread_join(thread, NULL); */
    CloseWindow();
}
