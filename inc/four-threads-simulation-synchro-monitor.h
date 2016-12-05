#ifndef __FOUR_THREAD_SIMULATION_SYNCHRO_MONITOR__
#define __FOUR_THREAD_SIMULATION_SYNCHRO_MONITOR__

#ifdef GUI
  #include <SDL2/SDL.h>
#endif

#include "simulation.h"
#include "field.h"

void dispatch_synchro_monitor(struct grid* field, std::list<int> exiting[], std::list<int> responsability[]);
void* four_threads_simulation_synchro_monitor(void* ptr_args);

#ifdef GUI
  void start_four_threads_simulation_synchro_monitor(grid* field, SDL_Renderer* renderer);
#else
  void start_four_threads_simulation_synchro_monitor(grid* field);
#endif // GUI


#endif // __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__
