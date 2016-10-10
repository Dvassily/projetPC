#ifndef SIMULATION_H
#define SIMULATION_H


#ifdef GUI
  #include <SDL2/SDL.h>
  #include "gui.h"
#endif // GUI

#define NB_MOVES 5

typedef enum {
    UNKNOWN_DIR = -1,
    N, NW, W, SW, S
} direction;

typedef enum {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
} field_zone;

typedef enum {
    UNKNOWN_SCE = -1,
    ONE_THREAD, FOUR_THREADS, N_THREADS
} scenario;

typedef struct {
    int x;
    int y;
} move;

typedef struct {
    field_zone zone;
    int person_id;
    #ifdef GUI
      SDL_Window* window;
      SDL_Renderer* renderer;
      struct rgb_color* colors;
    #endif // GUI
} thread_args;

void start_simulation(unsigned population, scenario sc, int print_duration);
double distance_to_azimuth(int x, int y);

#endif // SIMULATION_H
