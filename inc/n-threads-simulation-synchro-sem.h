#ifndef __N_THREAD_SIMULATION_SYNCHRO_SEM__
#define __N_THREAD_SIMULATION_SYNCHRO_SEM__

#include <SDL2/SDL.h>
#include "simulation.h"
#include "field.h"

void* n_threads_simulation_synchro_sem(void* ptr_args);

#ifdef GUI
void start_n_threads_simulation_synchro_sem(grid* field, SDL_Renderer* renderer);
#else
void start_n_threads_simulation_synchro_sem(grid* field);
#endif // GUI


#endif // __N_THREAD_SIMULATION_SYNCHRO_SEM__
