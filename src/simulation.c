#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
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

    // Set the old cells to FIELD->PEOPLE[P]
    for (int i = x; i < x + 4; ++i) {
	for (int j = y; j < y + 4; ++j) {
	    /*
	    switch(field->matrix[i][j].content) {
	    case PERSON:
		printf("P"); break;
	    case WALL:
		printf("W"); break;
	    case EMPTY:
		printf(" "); break;
		}*/
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

/* Return true if the simulation is finished (i.e : all persons reached the hole)
   and false otherwise */
int is_finished(grid * field) {
    for (int p = 0; p < field->person_count; ++p)
	if (field->people[p].status == IN)
	    return 0;
    
    return 1;
}

void one_thread_simulation(thread_args args) {
    #ifdef GUI
      SDL_Renderer * renderer = args.renderer;
      rgb_color* colors = args.colors;
    #endif
    
    int i = 0;
    while (! is_finished(&field)) {
	++i;

	for (int p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN) {
		double min_dist = 0;
		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    if (can_move(&field, field.people[p], m)) {
			int x = field.people[p].origin_x + moves[m].x;
			int y = field.people[p].origin_y + moves[m].y;

			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = m;
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
	while (SDL_PollEvent(&e) != 0) {
	    if( e.type == SDL_QUIT ) {
		return;
	    }
	}
	
	// Fill window in white
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	update(renderer, &field, colors);
	SDL_RenderPresent(renderer);
	#endif
    }

    printf("i = %d\n", i);
}

void* four_threads_simulation(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    printf("%d\n", zone);

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
    
    int i = 0;
    while (! is_finished(&field)) {
	++i;

	for (int p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN &&
		field.people[p].origin_x >= xmin && field.people[p].origin_x <= xmax &&
		field.people[p].origin_y >= ymin && field.people[p].origin_y <= ymax) {
		double min_dist = 0;
		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    int x = field.people[p].origin_x + moves[m].x;
		    int y = field.people[p].origin_y + moves[m].y;

		    if (can_move(&field, field.people[p], m)) {
			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = m;
			}
		    }
		}

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(&field, p, dir);
		printf("p%d : (%d, %d)\n", p, field.people[p].origin_x, field.people[p].origin_y);
		
		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		}
	    }
	}

	SDL_Delay(25);
    }

    printf("i = %d\n", i);

    return NULL;
}

void* n_threads_simulation(void* ptr_args)
{
    int person_id = ((thread_args*) ptr_args)->person_id;
    printf("%d\n", person_id);

    int i = 0;
    while (! is_finished(&field)) {
	++i;

	for (int p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN && p == person_id) {
		double min_dist = 0;
		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    int x = field.people[p].origin_x + moves[m].x;
		    int y = field.people[p].origin_y + moves[m].y;

		    if (can_move(&field, field.people[p], m)) {
			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = m;
			}
		    }
		}

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(&field, p, dir);
		printf("p%d : (%d, %d)\n", p, field.people[p].origin_x, field.people[p].origin_y);
		
		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		}
	    }
	}
	SDL_Delay(25);

    }

    printf("i = %d\n", i);
    
    return NULL;
}

void start_simulation(unsigned population, scenario sc) {
    int thread_status = 0;
    /*
    int person_id[population];
    for (unsigned i = 0; i < population; ++i)
	person_id[i] = i;
    */
    
    #ifdef GUI
      rgb_color colors[population];
      generate_colors(colors, population);

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

    if (sc == ONE_THREAD) {
	thread_args args;
	#ifdef GUI
	  args.renderer = renderer;
	  args.colors = colors;
	#endif
	one_thread_simulation(args);
    } else if (sc == FOUR_THREADS) {
	// BUG : Des fois 'segmentation fault', d'autres fois boucle infinie -> Pourquoi?
	//field_zone zones[4] = {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};
	pthread_t thread[4];
	thread_args t_args[4];
	
	for (unsigned i = 0; i < 4; ++i) {
	    t_args[i].zone = i;

	    thread_status = pthread_create(&thread[i], NULL,
					   &four_threads_simulation, (void*) &t_args[i]);

	    if (thread_status) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	    }
	}

#ifdef GUI
	while (! is_finished(&field)) {
	    while (SDL_PollEvent(&e) != 0) {
		if( e.type == SDL_QUIT ) {
		    return;
		}
	    }
	
	    // Fill window in white
	    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	    SDL_RenderClear(renderer);
	    update(renderer, &field, colors);
	    SDL_RenderPresent(renderer);
	}
#endif


	for (unsigned i = 0; i < 4; ++i)
	    pthread_join(thread[i], NULL); // retval?
    } else if (sc == N_THREADS) {
	thread_args t_args[population];
	pthread_t thread[population];
	
	for (unsigned i = 0; i < population; ++i) {
	    t_args[i].person_id = i;
	    thread_status = pthread_create(&thread[i], NULL,
					   &n_threads_simulation, (void*) &t_args[i]);
	    
	    if (thread_status) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	    }
	}

#ifdef GUI
	while (! is_finished(&field)) {
	    while (SDL_PollEvent(&e) != 0) {
		if( e.type == SDL_QUIT ) {
		    return;
		}
	    }
	
	    // Fill window in white
	    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	    SDL_RenderClear(renderer);
	    update(renderer, &field, colors);
	    SDL_RenderPresent(renderer);
	}
#endif
	for (unsigned i = 0; i < population; ++i)
	    pthread_join(thread[i], NULL); // retval?
    } else {
	fprintf(stderr, "Unknown scenario : %d\n", sc);
	exit(EXIT_FAILURE);
    }

#ifdef GUI
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    SDL_Quit();
#endif
}
