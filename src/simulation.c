#include <pthread.h>
#include "../inc/simulation.h"
#include "../inc/field.h"

grid field;


void * one_thread_simulation(void *arg) {
    
    return NULL;
}

void start_simulation(unsigned population, scenario sc) {
    
    // Field initialisation
    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
    populate_field(&field, population);

    switch (sc) {
    case ONE_THREAD: ;
	pthread_t thread;
	pthread_create(&thread, NULL, &one_thread_simulation, NULL);
	break;
    default:
	fprintf(stderr, "Unknown scenario : %d\n", sc);
    }
}
