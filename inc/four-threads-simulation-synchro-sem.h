#ifndef __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__
#define __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__

#include <SDL2/SDL.h>
#include "simulation.h"
#include "field.h"

void dispatch_synchro_sem(struct grid* field, std::list<int> exiting[], std::list<int> responsability[], sem_t exiting_lock[4]);
void* four_threads_simulation_synchro_sem(void* ptr_args);

#ifdef GUI
void start_four_threads_simulation_synchro_sem(grid* field, SDL_Renderer* renderer);
#else
void start_four_threads_simulation_synchro_sem(grid* field);
#endif // GUI


#endif // __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__
