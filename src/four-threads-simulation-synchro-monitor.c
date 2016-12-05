#include <pthread.h>
#include "../inc/n-threads-simulation-synchro-monitor.h"
#include "../inc/four-threads-simulation-no-synchro.h"
#include "../inc/monitor.h"

//monitor end_of_thread[NB_ZONE];

void* four_threads_simulation_synchro_monitor(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    grid* field = ((thread_args*) ptr_args)->field;
    std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    std::list<int>* incoming = ((thread_args*) ptr_args)->incoming;
    struct monitor* incoming_monitor = ((thread_args*) ptr_args)->incoming_monitor;
    std::list<int>::iterator iter;
    
    int xmin = get_xmin(zone);
    int xmax = get_xmax(zone);

    while (! is_finished(field)) {
	for (iter = responsability[zone].begin(); iter != responsability[zone].end(); ++iter) {
	    int p = *iter;
	    
	    if (field->people[p].status == IN) {
		direction dir = UNKNOWN_DIR;
		int old_x = field->people[p].origin_x;
		int old_y = field->people[p].origin_y;
		
		move_prologue_monitor(old_x, old_y);

		if ((dir = choose_direction(field, p)) != UNKNOWN_DIR) {		

		    move_person(field, p, dir);
		    move_epilogue_monitor(old_x, old_y);
		    
		    if (field->people[p].origin_x == 0) {
			field->people[p].status = OUT;
			delete_person(field, p);
		    } else {
			if (field->people[p].origin_x < xmin || field->people[p].origin_x > xmax) {
			    responsability[zone].erase(iter++);
			    //sem_wait(&incoming_lock[zone - 1]);
			    incoming_monitor[zone - 1].get(&incoming_monitor[zone - 1]);
			      incoming[zone - 1].push_back(p);
			    incoming_monitor[zone - 1].release(&incoming_monitor[zone - 1]);
			    //sem_post(&incoming_lock[zone - 1]);
			}
		    }
		} else {
		    move_epilogue_monitor(old_x, old_y);
		}
	    }
	}

	if (zone != RIGHT) {
	    incoming_monitor[zone].get(&incoming_monitor[zone]);
	    while (! incoming[zone].empty()) {
		responsability[zone].push_back(incoming[zone].front());
		incoming[zone].pop_front();
	    }
	    incoming_monitor[zone].release(&incoming_monitor[zone]);
	}
	
	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }
    /*
    for (unsigned i = 0; i < NB_ZONE; i++) {
	end_of_thread[i].release(&end_of_thread[i]);
    }
    */
    return NULL;
}

#ifdef GUI
  void start_four_threads_simulation_synchro_monitor(grid* field, SDL_Renderer* renderer)
#else
  void start_four_threads_simulation_synchro_monitor(grid* field)
#endif // GUI
{
    int thread_status = 0;

    pthread_t thread[NB_ZONE];
    thread_args t_args[NB_ZONE];

    for (unsigned i = 0; i < DEFAULT_GRID_WIDTH; ++i)
	for (unsigned j = 0; j < DEFAULT_GRID_HEIGHT; ++j)
	    init_monitor(&field_monitor[i][j], 1);
    
    std::list<int> responsability[NB_ZONE];

    std::list<int> incoming[NB_ZONE - 1];

    struct monitor incoming_monitor[3];
    for (unsigned i = 0; i < NB_ZONE - 1; ++i) {
	init_monitor(&incoming_monitor[i], 1);
    }
    
    for (unsigned p = 0; p < field->person_count; ++p) {
	responsability[get_zone(field->people[p].origin_x)].push_back(p);
    }

    /*
    for (unsigned i = 0; i < NB_ZONE; i++) {
	init_monitor(&end_of_thread[i], 0);
    }
    */
    for (unsigned i = 0; i < NB_ZONE; ++i) {
	t_args[i].field = field;
	t_args[i].zone = (::field_zone) i;
	t_args[i].incoming = incoming;
	t_args[i].responsability = responsability;
	t_args[i].incoming_monitor = incoming_monitor;

	thread_status = pthread_create(&thread[i], NULL,
				       &four_threads_simulation_synchro_monitor, (void*) &t_args[i]);

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

    //for (unsigned i = 0; i < NB_ZONE; i++) {
    //end_of_thread[i].get(&end_of_thread[i]);
    //}
}
