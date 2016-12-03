#ifndef __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__
#define __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__

#include "simulation.h"
#include "field.h"

void dispatch_synchro_sem(struct grid* field, std::list<int> exiting[], std::list<int> responsability[], sem_t exiting_lock[4]);
void* four_threads_simulation_synchro_sem(void* ptr_args);
void start_four_threads_simulation_synchro_sem(grid* field);

#endif // __FOUR_THREAD_SIMULATIOFOUR_SYNCHRO_SEM__
