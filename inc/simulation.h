#ifndef SIMULATION_H
#define SIMULATION_H

typedef enum {
    UNKNOWN = -1,
    ONE_THREAD, FOUR_THREADS, N_THREADS
} scenario;

void start_simulation(unsigned population, scenario sc);

#endif // SIMULATION_H
