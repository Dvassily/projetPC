#include <pthread.h>
#include "../inc/four-threads-simulation-no-synchro.h"

int get_xmin(field_zone zone) {
    return (zone == LEFT)?         0 :
	   (zone == CENTER_LEFT)?  ZONE_LENGTH :
	   (zone == CENTER_RIGHT)? 2 * ZONE_LENGTH : 3 * ZONE_LENGTH;
}


int get_xmax(field_zone zone) {
    return (zone == LEFT)?         ZONE_LENGTH - 1 :
	   (zone == CENTER_LEFT)?  2 * ZONE_LENGTH -1 :
	   (zone == CENTER_RIGHT)? 3 * ZONE_LENGTH - 1 : DEFAULT_GRID_WIDTH - 1;
}

field_zone get_zone(int x) {
    return (x < ZONE_LENGTH)? LEFT :
	(x < 2 * ZONE_LENGTH)? CENTER_LEFT :
	(x < 3 * ZONE_LENGTH)? CENTER_RIGHT : RIGHT;
	
}

void* four_threads_simulation(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    grid* field = ((thread_args*) ptr_args)->field;
    std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    std::list<int>* incoming = ((thread_args*) ptr_args)->incoming;
    std::list<int>::iterator iter;
    
    int xmin = get_xmin(zone);
    int xmax = get_xmax(zone);

    while (! is_finished(field)) {
	for (iter = responsability[zone].begin(); iter != responsability[zone].end(); ++iter) {
	    int p = *iter;
	    
	    if (field->people[p].status == IN) {
		direction dir = UNKNOWN_DIR;

		if ((dir = choose_direction(field, p)) != UNKNOWN_DIR) {		

		    move_person(field, p, dir);
		    
		    if (field->people[p].origin_x == 0) {
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
	}
	    
	while ((zone != RIGHT) && (! incoming[zone].empty())) {
	    responsability[zone].push_back(incoming[zone].front());
	    incoming[zone].pop_front();
	}
	
	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }
    
    return NULL;
}

#ifdef GUI
  void start_four_threads_simulation_no_synchro(grid* field, SDL_Renderer* renderer)
#else
  void start_four_threads_simulation_no_synchro(grid* field)
#endif // GUI
{
    int thread_status = 0;

    pthread_t thread[NB_ZONE];
    thread_args t_args[NB_ZONE];
    std::list<int> incoming[NB_ZONE - 1];
    std::list<int> responsability[NB_ZONE];

    for (unsigned p = 0; p < field->person_count; ++p) {
	responsability[get_zone(field->people[p].origin_x)].push_back(p);
    }
    
    for (unsigned i = 0; i < NB_ZONE; ++i) {
	t_args[i].field = field;
	t_args[i].zone = (::field_zone) i;
	t_args[i].incoming = incoming;
	t_args[i].responsability = responsability;
	

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
		for (unsigned i = 0; i < NB_ZONE; ++i) {
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
