#ifndef SIMULATION_H
#define SIMULATION_H

#define AZIMUTH_X  -4
#define AZIMUTH_Y  63
#define NB_MOVES 3

typedef enum {
    NW, W, SW
} direction;

typedef enum {
    UNKNOWN = -1,
    ONE_THREAD, FOUR_THREADS, N_THREADS
} scenario;

typedef struct {
    int x;
    int y;
} move;

void start_simulation(unsigned population, scenario sc);
double distance_to_azimuth(int x, int y);

#endif // SIMULATION_H
