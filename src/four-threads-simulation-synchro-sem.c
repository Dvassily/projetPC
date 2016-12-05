#include <pthread.h>
#include "../inc/n-threads-simulation-synchro-sem.h"
#include "../inc/four-threads-simulation-no-synchro.h"

//sem_t field_lock[DEFAULT_GRID_WIDTH][DEFAULT_GRID_HEIGHT];

void* four_threads_simulation_synchro_sem(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    grid* field = ((thread_args*) ptr_args)->field;
    std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    std::list<int>* incoming = ((thread_args*) ptr_args)->incoming;
    std::list<int>::iterator iter;
    sem_t* incoming_lock = ((thread_args*) ptr_args)->incoming_lock;
    sem_t* end_of_thread = ((thread_args*) ptr_args)->end_of_thread_lock;
    
    int xmin = get_xmin(zone);
    int xmax = get_xmax(zone);
    
    while (! is_finished(field)) {
	for (iter = responsability[zone].begin(); iter != responsability[zone].end(); ++iter) {
	    int p = *iter;
	    
	    if (field->people[p].status == IN) {
		int old_x = field->people[p].origin_x;
		int old_y = field->people[p].origin_y;
		direction dir = UNKNOWN_DIR;
	    
		move_prologue(old_x, old_y);
		if ((dir = choose_direction(field, p)) != UNKNOWN_DIR) {		

		    move_person(field, p, dir);
		    move_epilogue(old_x, old_y);
		    
		    if (field->people[p].origin_x == 0) {
			field->people[p].status = OUT;
			delete_person(field, p);
		    } else {
			if (field->people[p].origin_x < xmin || field->people[p].origin_x > xmax) {
			    responsability[zone].erase(iter++);

			    sem_wait(&incoming_lock[zone - 1]);
			      incoming[zone - 1].push_back(p);
			    sem_post(&incoming_lock[zone - 1]);
			}
		    }
		} else {
		    move_epilogue(old_x, old_y);
		}
	    } 
	}

	if (zone != RIGHT) {
	    sem_wait(&incoming_lock[zone]);
	    while (! incoming[zone].empty()) {
		responsability[zone].push_back(incoming[zone].front());
		incoming[zone].pop_front();
	    }
	    sem_post(&incoming_lock[zone]);
	}
	
	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }

    sem_post(end_of_thread);
    
    return NULL;
}

#ifdef GUI
  void start_four_threads_simulation_synchro_sem(grid* field, SDL_Renderer* renderer)
#else
  void start_four_threads_simulation_synchro_sem(grid* field)
#endif // GUI
{
    int thread_status = 0;
    pthread_t thread[NB_ZONE];
    thread_args t_args[NB_ZONE];

    std::list<int> responsability[NB_ZONE];
    std::list<int> incoming[NB_ZONE - 1];
    sem_t incoming_lock[NB_ZONE - 1];
    for (unsigned i = 0; i < NB_ZONE - 1; ++i)
	sem_init(&incoming_lock[i], 0, 1);

    for (unsigned i = 0; i < DEFAULT_GRID_WIDTH; ++i)
	for (unsigned j = 0; j < DEFAULT_GRID_HEIGHT; ++j)
	    sem_init(&field_lock[i][j], 0, 1);
    
    for (unsigned p = 0; p < field->person_count; ++p) {
	responsability[get_zone(field->people[p].origin_x)].push_back(p);
    }

    sem_t end_of_thread[NB_ZONE];
    for (unsigned i = 0; i < NB_ZONE; ++i)
	sem_init(&end_of_thread[i], 0, 0);
    

#ifdef GUI
    pthread_t display_thread;
    display_thread_args display_t_args;
    display_t_args.field = field;
    display_t_args.renderer = renderer;
    pthread_create(&display_thread, NULL, &display_thread_function, (void*) &display_t_args);
#endif // GUI

    for (unsigned i = 0; i < NB_ZONE; ++i) {
	t_args[i].field = field;
	t_args[i].zone = (::field_zone) i;
	t_args[i].incoming = incoming;
	t_args[i].responsability = responsability;
	t_args[i].incoming_lock = incoming_lock;
	t_args[i].end_of_thread_lock = &end_of_thread[i];
	

	thread_status = pthread_create(&thread[i], NULL,
				       &four_threads_simulation_synchro_sem, (void*) &t_args[i]);

	if (thread_status) {
	    fprintf(stderr, "Error creating thread\n");
	    exit(EXIT_FAILURE);
	}
    }

    for (unsigned i = 0; i < NB_ZONE; ++i)
	sem_wait(&end_of_thread[i]);

    //for (unsigned i = 0; i < 4; ++i)
    //pthread_join(thread[i], NULL); // retval?

#ifdef GUI
    pthread_join(display_thread, NULL);
#endif // GUI
	
}
