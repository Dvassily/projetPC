#include <pthread.h>
#include "../inc/n-threads-simulation-no-synchro.h"

void* n_threads_simulation(void* ptr_args)
{
    grid* field = ((thread_args*) ptr_args)->field;
    unsigned person_id = ((thread_args*) ptr_args)->person_id;

    int i = 0;
    while (field->people[person_id].status == IN) {
	++i;

	for (unsigned p = 0; p < field->person_count; ++p) {
	    if (field->people[p].status == IN && p == person_id) {
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

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(field, p, dir);
		
		if (field->people[p].origin_x == 0) {
		    field->people[p].status = OUT;
		    delete_person(field, p);
		}
	    }
	}

	#ifdef GUI
	SDL_Delay(10);
	#endif
    }
    
    return NULL;
}

#ifdef GUI
  void start_n_threads_simulation_no_synchro(grid* field, SDL_Renderer* renderer)
#else
  void start_n_threads_simulation_no_synchro(grid* field)
#endif // GUI
{
    int thread_status = 0;
    
    thread_args t_args[field->person_count];
    pthread_t thread[field->person_count];

    #ifdef GUI
    pthread_t display_thread;
    display_thread_args display_t_args;
    display_t_args.field = field;
    display_t_args.renderer = renderer;
    pthread_create(&display_thread, NULL, &display_thread_function, (void*) &display_t_args);
    #endif // GUI

    
    for (unsigned i = 0; i < field->person_count; ++i) {
	t_args[i].person_id = i;
	t_args[i].field = field;
	thread_status = pthread_create(&thread[i], NULL,
				       &n_threads_simulation, (void*) &t_args[i]);
	    
	if (thread_status) {
	    fprintf(stderr, "Error creating thread\n");
	    exit(EXIT_FAILURE);
	}
    }

    for (unsigned i = 0; i < field->person_count; ++i)
	pthread_join(thread[i], NULL); // retval?

#ifdef GUI
    pthread_join(display_thread, NULL);
#endif // GUI
}
