// TODO : Change functions body
/*
    Elements.c contains all function needed to create a grid.
    How to init a grid:

    Somewhere in your code:
        int number_of_people = 32;
        grid simulation_grid; 
        //memset(&simulation_grid, 0, sizeof(grid));
        init_grid(&simulation_grid, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
        random_populate_grid(&simulation_grid, number_of_people);
*/

#include <stdbool.h>
#include <math.h>
#include "../inc/field.h"

void init_cell(cell* my_cell, int x, int y, cell_content_type content){
    //my_cell = (cell*)malloc (sizeof (cell));
    my_cell->x = x;
    my_cell->y = y;
    my_cell->content = content;
}

void add_cell_to_grid(grid* my_grid, int x, int y, cell my_cell){
    //printf("add cell to grid at x: %d, y:%d \n", my_cell->x, my_cell->y);
    my_grid->matrix[x][y] = my_cell;
}


void add_walls(grid* my_grid){
    //First WALL
    for(int x = DEFAULT_WALL_1_X; x < DEFAULT_WALL_WIDTH; x++){
        for(int y = 0; y < my_grid->height; y++){
            // exit
            if(y <  (my_grid->height / 2) - (DEFAULT_EXIT_HEIGHT_WALL_1 / 2) ||
               y >= (my_grid->height / 2) + (DEFAULT_EXIT_HEIGHT_WALL_1 / 2) ) {
                my_grid->matrix[x][y].content = WALL;
            }
        }
    }

    //Second WALL
    for(int x = 0; x < DEFAULT_WALL_WIDTH; x++){
        for(int y = 0; y < my_grid->height; y++){
            if(y <  (my_grid->height / 2) - (DEFAULT_EXIT_HEIGHT_WALL_2 / 2) ||
               y >= (my_grid->height / 2) + (DEFAULT_EXIT_HEIGHT_WALL_2 / 2)) {
                my_grid->matrix[x + DEFAULT_WALL_2_X][y].content = WALL;
            }
        }
    }
}

void init_grid(grid* my_grid, int width, int height){
    my_grid->height = height;
    my_grid->width = width;
    my_grid->person_count = 0;

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            cell new_cell;
            init_cell(&new_cell, x, y, EMPTY);
            add_cell_to_grid(my_grid, x, y, new_cell);
        }
    }
    
    add_walls(my_grid);
}

int is_cell_empty(grid* my_grid, int x, int y){
    if (x < 0 || x > my_grid->width  - DEFAULT_PEOPLE_SIZE - 1 ||
	y < 0 || y > my_grid->height - DEFAULT_PEOPLE_SIZE - 1)
	return -1;
    
    for (int i = x; i < x + DEFAULT_PEOPLE_SIZE; i++) {
	for (int j = y; j < y + DEFAULT_PEOPLE_SIZE; j++) {
            if (my_grid->matrix[i][j].content != EMPTY)
		return -1;
        }
    }

    return 0;
}

/* This function place some peapole at random place inside the left area after the 2 walls */
void populate_field(grid* field, int people) {
    int x, y;
    int min_x = DEFAULT_WALL_2_X + DEFAULT_WALL_WIDTH;
    int max_x = field->width - DEFAULT_PEOPLE_SIZE;
    int min_y = 0;
    int max_y = field->height - DEFAULT_PEOPLE_SIZE;

    /*
    person a_person;
    init_person(&a_person, 130, 12);
    add_person_to_grid(field, a_person);
    */
    
    for(int i = 0; i < people; i++) {
        do {
            x = rand() % (max_x - min_x + 1) + min_x; // 127; 
            y = rand() % (max_y - min_y + 1) + min_y; // 34; 
        } while (is_cell_empty(field, x, y) < 0);
	
        person a_person;
        init_person(&a_person, x, y);
        add_person_to_grid(field, a_person);
    }
}


void init_person(person* p, int x, int y){
    p->origin_x = x;
    p->origin_y = y;
    p->status = IN;
    p->red_color   = rand() % (255 - 50 + 1) + 50;
    p->green_color = rand() % (255 - 50 + 1) + 50;
    p->blue_color  = rand() % (255 - 50 + 1) + 50;
}

void delete_person(grid * grid, int person) {
    int x = grid->people[person].origin_x;
    int y = grid->people[person].origin_y;
    
    for (int i = x; i < x + DEFAULT_PEOPLE_SIZE; ++i) {
	for (int j = y; j < y + DEFAULT_PEOPLE_SIZE; ++j) {
	    grid->matrix[i][j].content = EMPTY;
	}
    }

    //grid->people[person].origin_x = 0;
    //grid->people[person].origin_y = 0;
}

void add_person_to_grid(grid* my_grid, person a_person){
    //printf("add person to grid at x: %d, y:%d \n", a_person->origin_x, a_person->origin_y);
    if (my_grid->person_count < DEFAULT_MAX_PEOPLE_IN_GRID) {
        my_grid->people[my_grid->person_count] = a_person;
        my_grid->person_count++;
        for(int i = 0; i < DEFAULT_PEOPLE_SIZE; i++) {
            for(int j = 0; j < DEFAULT_PEOPLE_SIZE; j++) {
                my_grid->matrix[a_person.origin_x+i][a_person.origin_y+j].content = PERSON;
            }
        }
    } else {
        perror("Unable to add a new person. Grid is full.");
    }
}

double distance_to_azimuth(int x, int y) {
    int dist_x = x - AZIMUTH_X;
    int dist_y = y - AZIMUTH_Y;
    return sqrt( dist_x * dist_x + dist_y * dist_y );
}

/*
void set_x(cell* my_cell, int x){
    my_cell->x = x;
}

void set_y(cell* my_cell, int y){
    my_cell->y = y;
}

void set_cell_content(cell* my_cell, cell_content_type content){
    my_cell->content = content;
}
*/    
