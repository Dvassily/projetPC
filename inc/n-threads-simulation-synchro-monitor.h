#ifndef __N_THREAD_SIMULATION_SYNCHRO_MONITOR__
#define __N_THREAD_SIMULATION_SYNCHRO_MONITOR__
#ifdef GUI
#include <SDL2/SDL.h>
#endif
#include "simulation.h"
#include "field.h"

extern struct monitor field_monitor[DEFAULT_GRID_WIDTH][DEFAULT_GRID_HEIGHT];

void* n_threads_simulation_synchro_monitor(void* ptr_args);
void move_prologue_monitor(int x, int y);
void move_epilogue_monitor(int x, int y);

#ifdef GUI
void start_n_threads_simulation_synchro_monitor(grid* field, SDL_Renderer* renderer);
#else
void start_n_threads_simulation_synchro_monitor(grid* field);
#endif // GUI


#endif // __N_THREAD_SIMULATION_SYNCHRO_MONITOR__
