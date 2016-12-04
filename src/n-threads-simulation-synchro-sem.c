#include <pthread.h>
#include <fcntl.h>
#include "../inc/n-threads-simulation-no-synchro.h"

#define AREA_LOCK 4;

sem_t field_lock[DEFAULT_GRID_WIDTH][DEFAULT_GRID_HEIGHT];

void move_prologue(int x, int y) {
    for (int i = x + 3;  i >= x; --i) {
	sem_wait(&field_lock[i][y - 1]);
	sem_wait(&field_lock[i][y + 4]);
    }

    for (int i = y - 1; i <= y + 4; ++i) {
	sem_wait(&field_lock[x - 1][i]);	
    }
}

void move_epilogue(int x, int y) {
    for (int i = x + 3;  i >= x; --i) {
	sem_post(&field_lock[i][y - 1]);
	sem_post(&field_lock[i][y + 4]);
    }

    
    for (int i = y - 1; i <= y + 4; ++i) {
	sem_post(&field_lock[x - 1][i]);	
    }
}

void unlock_exit(person p) {
    for (int i = 0; i < 4; ++i) {
	for (int j = 0; j < 4; ++j) {
	    sem_post(&field_lock[p.origin_x + i][p.origin_y + j]);
	}
    }
}

void* n_threads_simulation_synchro_sem(void* ptr_args)
{
    grid* field = ((thread_args*) ptr_args)->field;
    unsigned p = ((thread_args*) ptr_args)->person_id;

    int i = 0;
    while (! is_finished(field) && field->people[p].status == IN) {
	++i;

	if (field->people[p].status == IN && p == p) {
	    double min_dist = distance_to_azimuth(field->people[p].origin_x,
						  field->people[p].origin_y);
	    direction dir = UNKNOWN_DIR;

	    int old_x = field->people[p].origin_x;
	    int old_y = field->people[p].origin_y;

	    move_prologue(old_x, old_y);
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

	    if (dir == UNKNOWN_DIR) {
		move_epilogue(old_x, old_y);
		continue;
	    }

	    move_person(field, p, dir);
		
	    if (field->people[p].origin_x == 0) {
		field->people[p].status = OUT;
		delete_person(field, p);
	    }

	    move_epilogue(old_x, old_y);
	}

	#ifdef GUI
	SDL_Delay(10);
	#endif
    }
    
    return NULL;
}

#ifdef GUI
  void start_n_threads_simulation_synchro_sem(grid* field, SDL_Renderer* renderer)
#else
  void start_n_threads_simulation_synchro_sem(grid* field)
#endif // GUI
{
    int thread_status = 0;
    
    thread_args t_args[field->person_count];
    pthread_t thread[field->person_count];

    for (unsigned i = 0; i < DEFAULT_GRID_WIDTH; ++i)
	for (unsigned j = 0; j < DEFAULT_GRID_HEIGHT; ++j)
	    sem_init(&field_lock[i][j], 0, 1);

    for (unsigned i = 0; i < field->person_count; ++i) {
	t_args[i].person_id = i;
	t_args[i].field = field;
	
	thread_status = pthread_create(&thread[i], NULL,
				       &n_threads_simulation_synchro_sem, (void*) &t_args[i]);
	    
	if (thread_status) {
	    fprintf(stderr, "Error creating thread\n");
	    exit(EXIT_FAILURE);
	}
    }

#ifdef GUI
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
	if( e.type == SDL_QUIT ) {
	    for (unsigned i = 0; i < field->person_count; ++i) {
		pthread_cancel(thread[i]);
	    }
	    return;
	}
    }

    while (! is_finished(field)) {	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	update(renderer, field);

	for (unsigned i = 0; i < DEFAULT_GRID_WIDTH; ++i) {
	    for (unsigned j = 0; j < DEFAULT_GRID_HEIGHT; ++j) {
		int value; sem_getvalue(&field_lock[i][j], &value);
		if (field->matrix[i][j].content == EMPTY && value == 0) {
		    SDL_Rect rectToDraw = {2 * i, 2 * j, 2, 2};
		    SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
		    SDL_RenderFillRect(renderer, &rectToDraw);	
		}
	    }
	}

	
	SDL_RenderPresent(renderer);
    }

#endif
    for (unsigned i = 0; i < field->person_count; ++i)
	pthread_join(thread[i], NULL); // retval?

}
