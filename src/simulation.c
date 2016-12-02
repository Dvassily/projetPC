#include <pthread.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <list>
#include "../inc/simulation.h"
#include "../inc/field.h"

#ifdef GUI
  #include "../inc/gui.h"
  #include <SDL2/SDL.h>

  SDL_Event e;
#endif

grid field;
move moves[] = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}
};

void dispatch(std::list<int> exiting[], std::list<int> responsability[]) {
    std::list<int>::iterator iter;
    
    for (unsigned i = 0; i < 4; ++i) {
	//for (unsigned j = 0; j < exiting[i].size(); ++j) {
	for (iter = exiting[i].begin(); iter != exiting[i].end(); ++iter) {
	    int index = *iter;
	    person p = field.people[index];
	    int zone = (int) (p.origin_x < DEFAULT_GRID_WIDTH / 2)?
		((p.origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_LEFT : BOTTOM_LEFT) :
		((p.origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_RIGHT : BOTTOM_RIGHT);
	    responsability[zone].push_front(index);
	    exiting[i].erase(iter++);
	}
    }
}

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

void one_thread_simulation(
#ifdef GUI
			   SDL_Renderer* renderer
#endif // GUI
			   )
{    
    int i = 0;
    while (! is_finished(&field)) {
	++i;

	for (unsigned p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN) {
		direction dir = choose_direction(&field, p);
		
		if (dir == UNKNOWN_DIR)
		    continue;
		
		move_person(&field, p, dir);
		
		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		}
	    }
	}

	#ifdef GUI
	while (SDL_PollEvent(&e) != 0)
	    if( e.type == SDL_QUIT )
		return;
	
	update(renderer, &field);
	#endif
    }
}

void* four_threads_simulation(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    std::list<int>* exiting = ((thread_args*) ptr_args)->exiting;
    std::list<int>::iterator iter;

    int xmin, xmax, ymin, ymax;
    
    switch(zone) {
    case TOP_LEFT:
	xmin = 0; xmax = (DEFAULT_GRID_WIDTH / 2) - 1;
	ymin = 0; ymax = (DEFAULT_GRID_HEIGHT / 2) - 1;
	break;
    case TOP_RIGHT:
	xmin = (DEFAULT_GRID_WIDTH / 2); xmax =  DEFAULT_GRID_WIDTH - 1;
	ymin = 0; ymax = (DEFAULT_GRID_HEIGHT / 2) - 1;
	break;
    case BOTTOM_LEFT:
	xmin = 0; xmax = (DEFAULT_GRID_WIDTH / 2) - 1;
	ymin = (DEFAULT_GRID_HEIGHT / 2) - 1; ymax = DEFAULT_GRID_HEIGHT - 1;
	break;
    case BOTTOM_RIGHT:
	xmin = (DEFAULT_GRID_WIDTH / 2); xmax =  DEFAULT_GRID_WIDTH - 1;
	ymin = (DEFAULT_GRID_HEIGHT / 2) - 1; ymax = DEFAULT_GRID_HEIGHT - 1;
	break;
    }
    
    for (unsigned p = 0; p < field.person_count; ++p) {
	field_zone person_zone = (::field_zone) (field.people[p].origin_x < DEFAULT_GRID_WIDTH / 2)?
	    ((field.people[p].origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_LEFT : BOTTOM_LEFT) :
	    ((field.people[p].origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_RIGHT : BOTTOM_RIGHT);
	
	if (zone == person_zone) {
	    responsability->push_back(p);
	}
    }

    while (! is_finished(&field)) {
	//printf("%d : %d\n", zone, responsability->size());
	for (iter = responsability->begin(); iter != responsability->end(); ++iter) {
	    //printf("%d\n", zone);
	    int p = *iter;
	    
	    if (field.people[p].status == IN) {
		double min_dist = distance_to_azimuth(field.people[p].origin_x,
						      field.people[p].origin_y);

		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    int x = field.people[p].origin_x + moves[m].x;
		    int y = field.people[p].origin_y + moves[m].y;

		    if (can_move(&field, field.people[p], (::direction) m)) {
			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = (::direction) m;
			}
		    }
		}

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(&field, p, dir);

		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		} else {
		    if (field.people[p].origin_x < xmin || field.people[p].origin_x > xmax ||
			field.people[p].origin_y < ymin || field.people[p].origin_y > ymax) {
			responsability->erase(iter++);
			exiting->push_front(p);
		    }
			    
		}
	    }
	}

	
	
	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }

    return NULL;
}

void* n_threads_simulation(void* ptr_args)
{
    unsigned person_id = ((thread_args*) ptr_args)->person_id;

    int i = 0;
    while (! is_finished(&field) && field.people[person_id].status == IN) {
	++i;

	for (unsigned p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN && p == person_id) {
		double min_dist = distance_to_azimuth(field.people[p].origin_x,
						      field.people[p].origin_y);
		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    if (can_move(&field, field.people[p], (::direction) m)) {
			int x = field.people[p].origin_x + moves[m].x;
			int y = field.people[p].origin_y + moves[m].y;

			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = (::direction) m;
			}
		    }
		}

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(&field, p, dir);
		
		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		}
	    }
	}

	#ifdef GUI
	SDL_Delay(10);
	#endif
    }
    
    return NULL;
}

void start_simulation(unsigned population, scenario sc, int print_duration) {
    int thread_status = 0;
    //clock_t timecpu_before = 0;
    //time_t timeuser_before = 0;
    struct timeval start, end;
    struct rusage usage;
    
    #ifdef GUI
      SDL_Window* window = NULL;
      SDL_Renderer* renderer = NULL;
      
      if (! init(&window, &renderer)) {
	  printf("GUI initialization failed.\n");
	  exit(EXIT_FAILURE);
      }
    #endif
    
    // Field initialisation
    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
    populate_field(&field, population);

    if (print_duration) {
	//timeuser_before = time(NULL);
	//timecpu_before = clock();
	getrusage(RUSAGE_SELF, &usage);
	start = usage.ru_utime;
    }
    
    if (sc == ONE_THREAD) {
	one_thread_simulation(
#ifdef GUI
			      renderer
#endif // GUI
			      );
    } else if (sc == FOUR_THREADS) {
	pthread_t thread[4];
	thread_args t_args[4];
	std::list<int> responsability[4];
	std::list<int> exiting[4];

	for (unsigned i = 0; i < 4; ++i) {
	    t_args[i].zone = (::field_zone) i;
	    t_args[i].responsability = &responsability[i];
	    t_args[i].exiting = &exiting[i];
	    
	    thread_status = pthread_create(&thread[i], NULL,
					   &four_threads_simulation, (void*) &t_args[i]);

	    if (thread_status) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	    }
	}

	while (! is_finished(&field)) {
	    dispatch(exiting, responsability);
#ifdef GUI
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
	    update(renderer, &field);
	    SDL_RenderPresent(renderer);
#endif
	}

	for (unsigned i = 0; i < 4; ++i)
	    pthread_join(thread[i], NULL); // retval?
    } else if (sc == N_THREADS) {
	thread_args t_args[field.person_count];
	pthread_t thread[field.person_count];

	for (unsigned i = 0; i < field.person_count; ++i) {
	    t_args[i].person_id = i;
	    thread_status = pthread_create(&thread[i], NULL,
					   &n_threads_simulation, (void*) &t_args[i]);
	    
	    if (thread_status) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	    }
	}

#ifdef GUI
	while (SDL_PollEvent(&e) != 0) {
	    if( e.type == SDL_QUIT ) {
		for (unsigned i = 0; i < field.person_count; ++i) {
		    pthread_cancel(thread[i]);
		}
		return;
		
	    }
	}

	while (! is_finished(&field)) {	
	    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	    SDL_RenderClear(renderer);
	    update(renderer, &field);
	    SDL_RenderPresent(renderer);
	}
#endif
	for (unsigned i = 0; i < field.person_count; ++i)
	    pthread_join(thread[i], NULL); // retval?
    } else {
	fprintf(stderr, "Unknown scenario : %d\n", sc);
	exit(EXIT_FAILURE);
    }

    if (print_duration) {
	getrusage(RUSAGE_SELF, &usage);
	end = usage.ru_utime;
	//printf("Execution time : \n");
	//printf("CPU : %f\n", (float) (clock() - timecpu_before) / CLOCKS_PER_SEC);
	//printf("%f\n", (float) (clock() - timecpu_before) / CLOCKS_PER_SEC);
	printf("Total time taken by CPU: %fs\n", (double)(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6);
	//printf("User : %f\n", (float) (time(NULL) - timeuser_before));
    }

#ifdef GUI
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    SDL_Quit();
#endif
}
