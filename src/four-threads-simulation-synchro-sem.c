#include <pthread.h>
#include "../inc/four-threads-simulation-no-synchro.h"

//std::list<int> incoming[3];
//std::list<int> responsability[4];

void* four_threads_simulation(void* ptr_args)
{
    /*  
    field_zone zone = ((thread_args*) ptr_args)->zone;
    grid* field = ((thread_args*) ptr_args)->field;
    //std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    //std::list<int>* incoming = ((thread_args*) ptr_args)->incoming;
    //std::list<int>* incoming_left = ((thread_args*) ptr_args)->incoming_left;
    std::list<int>::iterator iter;

    //printf("z%d : %d\n", zone, incoming_left);
    
    int xmin, xmax;

    unsigned zone_length = DEFAULT_GRID_WIDTH / 4;
    switch(zone) {
	
	// LEFT
    case TOP_LEFT:
	//xmin = 0; xmax = (DEFAULT_GRID_WIDTH / 2) - 1;
	xmin = 0; xmax = zone_length - 1;
	break;

	// MIDDLE LEFT
    case TOP_RIGHT:
	xmin = zone_length; xmax =  2 * zone_length - 1;
	break;

	// MIDDLE RIGHT
    case BOTTOM_LEFT:
	xmin = 2 * zone_length; xmax = 3 * zone_length - 1;
	break;

	// RIGHT
    case BOTTOM_RIGHT:
	xmin = 3 * zone_length; xmax =  DEFAULT_GRID_WIDTH - 1;
	break;
    }
    
    for (unsigned p = 0; p < field->person_count; ++p) {
	if (field->people[p].origin_x >= xmin && field->people[p].origin_x <= xmax) {
	    responsability[zone].push_back(p);
	}
    }
    
    while (! is_finished(field)) {
	for (iter = responsability[zone].begin(); iter != responsability[zone].end(); ++iter) {
	    int p = *iter;
	    
	    if (field->people[p].status == IN) {
		double min_dist = distance_to_azimuth(field->people[p].origin_x,
						      field->people[p].origin_y);

		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    int x = field->people[p].origin_x + moves[m].x;
		    int y = field->people[p].origin_y + moves[m].y;

		    if (can_move(field, field->people[p], (::direction) m)) {
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
		    printf("out\n");
		    field->people[p].status = OUT;
		    delete_person(field, p);
		} else {
		    if (field->people[p].origin_x < xmin || field->people[p].origin_x > xmax) {
			responsability[zone].erase(iter++);
			incoming[zone - 1].push_back(p);
		    }
		}
	    }
	}

	while (! incoming[zone].empty()) {
	    responsability[zone].push_back(incoming[zone].front());
	    incoming[zone].pop_front();
	    //incoming->pop_front();
	}

	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }
    */    
    return NULL;
}

#ifdef GUI
  void start_four_threads_simulation_no_synchro(grid* field, SDL_Renderer* renderer)
#else
  void start_four_threads_simulation_no_synchro(grid* field)
#endif // GUI
{
    int thread_status = 0;

    pthread_t thread[4];
    thread_args t_args[4];
    std::list<int> exiting[4];

    for (unsigned i = 0; i < 4; ++i) {
	t_args[i].field = field;
	t_args[i].zone = (::field_zone) i;
	//t_args[i].responsability = &responsability[i];
	//t_args[i].incoming = &incoming[i];
	/*
	if (i > 0) {
	    t_args[i].incoming_left = &incoming[i - 1];
	} else {
	    t_args[i].incoming_left = NULL;
	}
	*/	    
	thread_status = pthread_create(&thread[i], NULL,
				       &four_threads_simulation, (void*) &t_args[i]);

	if (thread_status) {
	    fprintf(stderr, "Error creating thread\n");
	    exit(EXIT_FAILURE);
	}
    }

#ifdef GUI
    while (! is_finished(field)) {
	//dispatch(field, exiting, responsability);
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0) {
	    if( e.type == SDL_QUIT ) {
		for (unsigned i = 0; i < 4; ++i) {
		    pthread_cancel(thread[i]);
		}
		return;
	    }
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	update(renderer, field);
	SDL_RenderPresent(renderer);
    }
#endif

    for (unsigned i = 0; i < 4; ++i)
	pthread_join(thread[i], NULL); // retval?
}
