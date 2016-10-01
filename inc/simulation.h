#ifndef SIMULATION_H
#define SIMULATION_H

#define NB_MOVES 5

typedef enum {
    UNKNOWN_DIR = -1,
    N, NW, W, SW, S
} direction;

typedef enum {
    UNKNOWN_SCE = -1,
    ONE_THREAD, FOUR_THREADS, N_THREADS
} scenario;

typedef struct {
    int x;
    int y;
} move;

void start_simulation(unsigned population, scenario sc);
double distance_to_azimuth(int x, int y);

#endif // SIMULATION_H
