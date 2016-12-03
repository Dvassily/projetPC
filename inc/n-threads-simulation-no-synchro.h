#ifndef __N_THREAD_SIMULATION_NO_SYNCHRO__
#define __N_THREAD_SIMULATION_NO_SYNCHRO__

#include "simulation.h"
#include "field.h"

void* n_threads_simulation(void* ptr_args);
void start_n_threads_simulation_no_synchro(grid* field);

#endif // __N_THREAD_SIMULATION_NO_SYNCHRO__
