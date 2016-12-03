#ifndef SIMULATION_H
#define SIMULATION_H

#include <list>
#include "field.h"

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

extern move moves[];

typedef struct {
    struct grid* field;
    field_zone zone;
    unsigned person_id;
    std::list<int> *responsability;
    std::list<int> *exiting;
    
    #ifdef GUI
      SDL_Window* window;
      SDL_Renderer* renderer;
    #endif // GUI
} thread_args;

void start_simulation(grid* field, scenario sc
#ifdef GUI
		      , SDL_Renderer* renderer
#endif
		      );
double distance_to_azimuth(int x, int y);
int is_finished(grid * field);
bool can_move(grid * field, struct person person, direction d);
void move_person(grid * field, unsigned p, direction d);

#ifdef GUI
#define START_FOUR_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_four_threads_simulation_no_synchro(field, renderer);
#define START_N_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_n_threads_simulation_no_synchro(field, renderer);
#else
#define START_FOUR_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_four_threads_simulation_no_synchro(field);
#define START_N_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_n_threads_simulation_no_synchro(field);
#endif // GUI



#endif // SIMULATION_H
