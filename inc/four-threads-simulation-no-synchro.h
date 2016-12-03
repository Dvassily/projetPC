#ifndef __FOUR_THREAD_SIMULATION_NO_SYNCHRO__
#define __FOUR_THREAD_SIMULATION_NO_SYNCHRO__

#include "simulation.h"
#include "field.h"

void dispatch(grid* field, std::list<int> exiting[], std::list<int> responsability[]);
void* four_threads_simulation(void* ptr_args);

#ifdef GUI
void start_four_threads_simulation_no_synchro(grid* field, SDL_Renderer* renderer);
#else
void start_four_threads_simulation_no_synchro(grid* field);
#endif // GUI

#endif // __FOUR_THREAD_SIMULATION_NO_SYNCHRO__
