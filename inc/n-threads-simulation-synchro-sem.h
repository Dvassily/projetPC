#ifndef __N_THREAD_SIMULATION_SYNCHRO_SEM__
#define __N_THREAD_SIMULATION_SYNCHRO_SEM__
#ifdef GUI
#include <SDL2/SDL.h>
#endif
#include "simulation.h"
#include "field.h"

//extern sem_t field_lock[DEFAULT_GRID_WIDTH][DEFAULT_GRID_HEIGHT];
extern sem_t field_lock[512][128];

void* n_threads_simulation_synchro_sem(void* ptr_args);
void move_prologue(int x, int y);
void move_epilogue(int x, int y);

#ifdef GUI
void start_n_threads_simulation_synchro_sem(grid* field, SDL_Renderer* renderer);
#else
void start_n_threads_simulation_synchro_sem(grid* field);
#endif // GUI


#endif // __N_THREAD_SIMULATION_SYNCHRO_SEM__
