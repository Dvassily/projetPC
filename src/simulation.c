#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/simulation.h"
#include "../inc/field.h"

grid field;
move moves[] = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}
};

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
		putchar('P');
		break;
	    }
	}
	putchar('\n');
    }
    putchar('\n');
}

double distance_to_azimuth(int x, int y) {
    int dist_x = x - AZIMUTH_X;
    int dist_y = y - AZIMUTH_Y;
    return sqrt( dist_x * dist_x + dist_y * dist_y );
}

void move_person(grid * field, unsigned person, direction d) {
    // New position of the person
    int x = field->people[person].origin_x + moves[d].x;
    int y = field->people[person].origin_y + moves[d].y;

    // Set the old cells to EMPTY
    for (int i = field->people[person].origin_x;
	 i < field->people[person].origin_x + 4; ++i)
	for (int j = field->people[person].origin_y;
	     j < field->people[person].origin_y + 4; ++j)
	    field->matrix[i][j].content = EMPTY;

    // Update person's position
    field->people[person].origin_x = x;
    field->people[person].origin_y = y;

    // Set the old cells to PERSON
    for (int i = x; i < x + 4; ++i)
	for (int j = y; j < y + 4; ++j)
	    field->matrix[i][j].content = PERSON;
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
    print_field();
    printf("\n\n\n\n\n\n");

    double min_dist = 0;
    direction dir = 0;
    int i = 0;
    printf("f : %d\n", is_finished(&field));
    while (! is_finished(&field)) {
	++i;
	
	for (int p = 0; p < field.person_count; ++p) {
	    printf("p : %d\n", p);
	    if (field.people[p].status == IN) {

		for (int m = 0; m < NB_MOVES; ++m) {
			    
		    int x = field.people[p].origin_x + moves[m].x;
		    int y = field.people[p].origin_y + moves[m].y;

		    switch(m) {
		    case 0:
			printf("N"); break;
		    case 1:
			printf("NW"); break;
		    case 2:
			printf("W"); break;
		    case 3:
			printf("SW"); break;
		    case 4:
			printf("S"); break;
		    }
		    printf("\n");

		    /*
		    switch(field.matrix[x][y].content) {
		    case EMPTY:
			printf("EMPTY\n");
			break;
		    case WALL:
			printf("WALL\n");
			break;
		    case PERSON:
			printf("PERSON\n");
			break;
			}*/

		    if (field.matrix[x][y].content == EMPTY) {
			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    printf("Ok\n");

			    min_dist = dist;
			    dir = m;
			}
		    }

		}
		
		move_person(&field, p, dir);

		if (field.people[p].origin_x == 0) {
		    field.people[p].status = OUT;
		}
	    }
	}

	if ((i % 10) == 0) {
	    print_field();
	    printf("\n\n\n\n\n\n");
	}
    }

    printf("i = %d\n", i);
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
    case FOUR_THREADS:
	// pthread_t thread[4];
	// thread_status = pthread_create(&thread, NULL, &one_thread_simulation, NULL);
	break;
    default:
	fprintf(stderr, "Unknown scenario : %d\n", sc);
	exit(EXIT_FAILURE);
    }

    if (sc != ONE_THREAD && thread_status) {
	fprintf(stderr, "Error creating thread\n");
	exit(EXIT_FAILURE);
    }
}
