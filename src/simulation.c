#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../inc/simulation.h"
#include "../inc/field.h"

grid field;
move moves[] = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}
};

/* Display the field */
void print_field() {
    printf("People : %d\n", field.person_count);
    printf("Field : \n");

    for (int i = 0; i < field.height; ++i) {
	for (int j = 0; j < field.width; ++j) {
	    switch (field.matrix[j][i].content) {
	    case EMPTY:
		putchar('_');
		break;
	    case WALL:
		putchar('W');
		break;
	    case PERSON:
		for (int p = 0; p < field.person_count; ++p) {
		    if ((j >= field.people[p].origin_x && j < field.people[p].origin_x + 4) &&
			(i >= field.people[p].origin_y && i < field.people[p].origin_y + 4)) {
			putchar('0' + p);
		    }
		}
		break;
	    }
	}
	putchar('\n');
    }
    putchar('\n');
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
	     j < field->people[p].origin_y + 4; ++j)
	    field->matrix[i][j].content = EMPTY;

    // Update field->people[p]'s position
    field->people[p].origin_x = x;
    field->people[p].origin_y = y;

    // Set the old cells to FIELD->PEOPLE[P]
    for (int i = x; i < x + 4; ++i)
	for (int j = y; j < y + 4; ++j)
	    field->matrix[i][j].content = PERSON;
}

/* Returns true if the person 'person' in the field 'field 'can move into direction 'd'  */
bool can_move(grid * field, person person, direction d) {
    // New position of the person
    int x = person.origin_x;
    int y = person.origin_y;

    if (d == N) {
	for (int i = x; i < x + 4; ++i)
	    if (field->matrix[x][y - 1].content != EMPTY) return false;

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

void one_thread_simulation() {
    print_field(); printf("\n\n\n\n\n\n");
    
    int i = 0;
    while (! is_finished(&field)) {
	++i;

	for (int p = 0; p < field.person_count; ++p) {
	    if (field.people[p].status == IN) {
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

		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		    delete_person(&field, p);
		}
	    }
	}
	
	if ((i % 5) == 0) {
	    print_field(); printf("\n\n\n\n\n\n");
	}
    }

    printf("i = %d\n", i);
}

void* four_threads_simulation(void* ptr_zone)
{
    field_zone zone = *((field_zone*) ptr_zone);
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
    }

    printf("i = %d\n", i);
    
    
    return NULL;
}

void start_simulation(unsigned population, scenario sc) {
    int thread_status = 0;
    // Field initialisation
    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
    populate_field(&field, population);

    switch (sc) {
    case ONE_THREAD: ;
	one_thread_simulation();
	break;
    case FOUR_THREADS: ;
	pthread_t thread[4];
	field_zone zones[4] = {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};
	
	for (int i = 0; i < 4; ++i) {
	    thread_status = pthread_create(&thread[i], NULL,
					   &four_threads_simulation, (void*) &zones[i]);

	    if (thread_status) {
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	    }
	}

	for (int i = 0; i < 4; ++i)
	    pthread_join(thread[i], NULL); // retval?
	
	break;
    default:
	fprintf(stderr, "Unknown scenario : %d\n", sc);
	exit(EXIT_FAILURE);
    }

}
