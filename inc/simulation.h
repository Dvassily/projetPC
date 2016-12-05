#ifndef SIMULATION_H
#define SIMULATION_H

#include <list>
#include <semaphore.h>
#include "field.h"
#include "../inc/four-threads-simulation-synchro-sem.h"
#include "../inc/n-threads-simulation-synchro-sem.h"

#ifdef GUI
  #include <SDL2/SDL.h>
  #include "gui.h"
#endif // GUI

#define NB_MOVES 5

typedef enum {
    UNKNOWN_DIR = -1,
    N, NW, W, SW, S
} direction;

typedef enum {
    LEFT, CENTER_LEFT, CENTER_RIGHT, RIGHT
} field_zone;

typedef enum {
    UNKNOWN_SCE = -1,
    ONE_THREAD, FOUR_THREADS, N_THREADS
} scenario;

typedef enum {
    UNKNOWN_STEP = 0,
    STEP_ONE, STEP_TWO, STEP_THREE
} step;


typedef struct {
    int x;
    int y;
} move;

extern move moves[];

typedef struct {
    struct grid* field;
    field_zone zone;
    unsigned person_id;
    // List of the index of persons who are under the responsability of current thread
    std::list<int> *responsability;

    // Semaphore to lock the modification of the 'responsability' list of the current thread
    sem_t* responsability_lock;

    // Array of lists of indexes of persons who are pending for being reassigned to a thread
    std::list<int> *incoming;

    // Semaphore to protect incoming list
    sem_t* incoming_lock;

    // Monitor to protect incoming list
    struct monitor* incoming_monitor;

    // Semaphore which prevent the main thread to progress until the four threads are not finished
    sem_t* end_of_thread_lock;

    // Monitor which prevent the main thread to progress until the four threads are not finished
    monitor* end_of_thread_monitor;

#ifdef GUI
      SDL_Window* window;
      SDL_Renderer* renderer;
    #endif // GUI
} thread_args;

void start_simulation(grid* field, scenario sc, step step
#ifdef GUI
		      , SDL_Renderer* renderer
#endif
		      );
double distance_to_azimuth(int x, int y);
int is_finished(grid * field);
bool can_move(grid * field, struct person person, direction d);
void move_person(grid * field, unsigned p, direction d);
direction choose_direction(grid * field, int p);

#ifdef GUI
// step 1
#define START_ONE_THREAD_SIMULATION(field, renderer)	\
    one_thread_simulation(field, renderer);
#define START_FOUR_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_four_threads_simulation_no_synchro(field, renderer);
#define START_N_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_n_threads_simulation_no_synchro(field, renderer);

// step 2
#define START_FOUR_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer)	\
    start_four_threads_simulation_synchro_sem(field, renderer);
#define START_N_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer)	\
    start_n_threads_simulation_synchro_sem(field, renderer);

// step 3
#define START_FOUR_THREADS_SIMULATION_SYNCHRO_MONITOR(field, renderer)	\
    start_four_threads_simulation_synchro_monitor(field, renderer);
#define START_N_THREADS_SIMULATION_SYNCHRO_MONITOR(field, renderer)	\
    start_n_threads_simulation_synchro_monitor(field, renderer);

#else

// step 1
#define START_ONE_THREAD_SIMULATION(field, renderer)	\
    one_thread_simulation(field);
#define START_FOUR_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_four_threads_simulation_no_synchro(field);
#define START_N_THREADS_SIMULATION_NO_SYNCHRO(field, renderer)	\
    start_n_threads_simulation_no_synchro(field);

// step 2
#define START_FOUR_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer)	\
    start_four_threads_simulation_synchro_sem(field);
#define START_N_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer)	\
    start_n_threads_simulation_synchro_sem(field);

// step 3
#define START_FOUR_THREADS_SIMULATION_SYNCHRO_MONITOR(field, renderer)	\
    start_four_threads_simulation_synchro_monitor(field);
#define START_N_THREADS_SIMULATION_SYNCHRO_MONITOR(field, renderer)	\
    start_n_threads_simulation_synchro_monitor(field);
#endif // GUI


#endif // SIMULATION_H
