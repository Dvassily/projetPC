#include <pthread.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <list>
#include "../inc/simulation.h"
#include "../inc/field.h"
#include "../inc/four-threads-simulation-no-synchro.h"
#include "../inc/n-threads-simulation-no-synchro.h"

#ifdef GUI
  #include "../inc/gui.h"
  #include <SDL2/SDL.h>

  SDL_Event e;
#endif

move moves[] = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}
};

/* Move a person in the field into a direction  */
void move_person(grid * field, unsigned p, direction d) {
    // New position of the field->people[p]
    int x = field->people[p].origin_x + moves[d].x;
    int y = field->people[p].origin_y + moves[d].y;

    // Set the old cells to EMPTY
    for (int i = field->people[p].origin_x;
	 i < field->people[p].origin_x + 4; ++i)
	for (int j = field->people[p].origin_y;
	     j < field->people[p].origin_y + 4; ++j) {
	    field->matrix[i][j].content = EMPTY;

	}

    // Update field->people[p]'s position
    field->people[p].origin_x = x;
    field->people[p].origin_y = y;

    // Set the old cells to PERSON
    for (int i = x; i < x + 4; ++i) {
	for (int j = y; j < y + 4; ++j) {
	    field->matrix[i][j].content = PERSON;
	}
    }
}

/* Returns true if the person 'person' in the field 'field 'can move into direction 'd'  */
bool can_move(grid * field, person person, direction d) {
    // New position of the person
    int x = person.origin_x;
    int y = person.origin_y;

    if (d == N) {
	for (int i = x; i < x + 4; ++i)
	    if (field->matrix[i][y - 1].content != EMPTY)
		return false;
    } else if (d == NW) {
	for (int i = y - 1; i < (y - 1) + 4; ++i)
	    if (field->matrix[x - 1][i].content != EMPTY) return false;
	for (int i = x; i < x + 3; ++i)
	    if (field->matrix[i][y- 1].content != EMPTY) return false;
    } else if (d == W) {
	for (int i = y; i < y + 4; ++i)
	    if (field->matrix[x - 1][i].content != EMPTY) return false;
	
    } else if (d == SW) {
	for (int i = y + 1; i < (y + 1) + 4; ++i)
	    if (field->matrix[x - 1][i].content != EMPTY) return false;
	for (int i = x; i < x + 3; ++i)
	    if (field->matrix[i][y + 4].content != EMPTY) return false;
	
    } else if (d == S) {
	for (int i = x; i < x + 4; ++i)
	    if (field->matrix[i][y + 4].content != EMPTY) return false;
	
    } else if (d == UNKNOWN_DIR)
	return false;
    
    return true;
}

direction choose_direction(grid * field, int p) {
    double min_dist = distance_to_azimuth(field->people[p].origin_x,
					  field->people[p].origin_y);
    direction dir = UNKNOWN_DIR;
		
    for (int m = 0; m < NB_MOVES; ++m) {
	if (can_move(field, field->people[p], (::direction) m)) {
	    int x = field->people[p].origin_x + moves[m].x;
	    int y = field->people[p].origin_y + moves[m].y;

	    float dist = distance_to_azimuth(x, y);
			
	    if (min_dist == 0 || dist < min_dist) {
		min_dist = dist;
		dir = (::direction) m;
	    }
	}
    }

    return dir;
}


/* Return true if the simulation is finished (i.e : all persons reached the hole)
   and false otherwise */
int is_finished(grid * field) {
    for (unsigned p = 0; p < field->person_count; ++p)
	if (field->people[p].status == IN)
	    return 0;
    
    return 1;
}

void one_thread_simulation(grid* field
                           #ifdef GUI
			     , SDL_Renderer* renderer
                           #endif // GUI
			   )
{    
    while (! is_finished(field)) {
	for (unsigned p = 0; p < field->person_count; ++p) {
	    if (field->people[p].status == IN) {
		direction dir = UNKNOWN_DIR;

		if ((dir = choose_direction(field, p)) != UNKNOWN_DIR) {
		    move_person(field, p, dir);
		    
		    if (field->people[p].origin_x == 0) {
			field->people[p].status = OUT;
			delete_person(field, p);
		    }
		}
	    }
	}

#ifdef GUI
	SDL_Delay(10);
	
	while (SDL_PollEvent(&e) != 0)
	    if( e.type == SDL_QUIT )
		return;
	
	update(renderer, field);
	#endif
    }
}

void start_simulation(grid* field, scenario sc, step step
                      #ifdef GUI
		      , SDL_Renderer* renderer
                      #endif
		      ) {
    if (step == STEP_ONE) {
	if (sc == ONE_THREAD) {
	    START_ONE_THREAD_SIMULATION(field, renderer);
	} else if (sc == FOUR_THREADS) {
	    START_FOUR_THREADS_SIMULATION_NO_SYNCHRO(field, renderer);
	} else if (sc == N_THREADS) {
	    START_N_THREADS_SIMULATION_NO_SYNCHRO(field, renderer);
	} else {
	    fprintf(stderr, "Unknown scenario : %d\n", sc);
	    exit(EXIT_FAILURE);
	}
    } else if (step == STEP_TWO) {
	if (sc == ONE_THREAD) {
	    START_ONE_THREAD_SIMULATION(field, renderer);
	} else if (sc == FOUR_THREADS) {
	    START_FOUR_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer);
	} else if (sc == N_THREADS) {
	    START_N_THREADS_SIMULATION_SYNCHRO_SEM(field, renderer);
	} else {
	    fprintf(stderr, "Unknown scenario : %d\n", sc);
	    exit(EXIT_FAILURE);
	}
    }
}
