#ifndef __GUI_H__
#define __GUI_H__

#include "../inc/field.h"
#include <stdbool.h>
#ifdef GUI
#include <SDL2/SDL.h>

typedef struct rgb_color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} rgb_color;

typedef struct display_thread_args {
    struct grid* field;
    SDL_Renderer* renderer;
} display_thread_args;

void* display_thread_function(void* ptr_args);
void generate_colors(rgb_color colors[], int number);
bool init(SDL_Window** window, SDL_Renderer** renderer);
void update(SDL_Renderer* renderer, struct grid* field);

#endif
#endif // __GUI_H__
