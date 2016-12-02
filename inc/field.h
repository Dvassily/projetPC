/*
Author Matthieu BERGER
15/09/2016

EZConcurrency Project
*/
#ifndef FIELD_H
#define FIELD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulation.h"

#define DEFAULT_GRID_WIDTH              512
#define DEFAULT_GRID_HEIGHT             128
#define DEFAULT_WALL_WIDTH              16
#define DEFAULT_EXIT_HEIGHT_WALL_1      8
#define DEFAULT_EXIT_HEIGHT_WALL_2      16
#define DEFAULT_WALL_1_X                0
#define DEFAULT_WALL_2_X                111
#define DEFAULT_PEOPLE_SIZE             4
#define DEFAULT_MAX_PEOPLE_IN_GRID      512
#define AZIMUTH_X  0
#define AZIMUTH_Y  64

/*
#define DEFAULT_GRID_WIDTH              192
#define DEFAULT_GRID_HEIGHT             64
#define DEFAULT_WALL_WIDTH              16
#define DEFAULT_EXIT_HEIGHT_WALL_1      8
#define DEFAULT_EXIT_HEIGHT_WALL_2      16
#define DEFAULT_WALL_1_X                0
#define DEFAULT_WALL_2_X                111
#define DEFAULT_PEOPLE_SIZE             4
#define DEFAULT_MAX_PEOPLE_IN_GRID      512
#define AZIMUTH_X  -4
#define AZIMUTH_Y  31
*/

//Cell content type enum
typedef enum {
    EMPTY,
    WALL,
    PERSON,
} cell_content_type;

//Cell structure
typedef struct {
    int x;
    int y;
    cell_content_type content;
} cell;

typedef enum {
    IN, OUT
} person_status;

/* Person structure
 Person is represented like this on a grid:
 _____
 |_|_|
 |_|_|
 The origin point of a person is located in the upper-left corner
 */
typedef struct {
    int origin_x;
    int origin_y;
    person_status status;
    unsigned short red_color;
    unsigned short green_color;
    unsigned short blue_color;
} person;

//Grid structure
typedef struct grid {
    int height;
    int width;
    cell matrix[DEFAULT_GRID_WIDTH][DEFAULT_GRID_HEIGHT];
    unsigned person_count;
    person people[DEFAULT_MAX_PEOPLE_IN_GRID];
} grid;

void init_person(person* a_person, int x, int y);
void add_person_to_grid(grid* my_grid, person a_person);
void init_grid(grid* my_grid, int width, int height);
void populate_field(grid* field, int people);
int is_cell_empty(grid* my_grid, int x, int y);
double distance_to_azimuth(int x, int y);
void delete_person(grid * grid, int person);

/*
void set_cell_content(cell* my_cell, cell_content_type content);
void init_cell(cell* my_cell, int x, int y, cell_content_type content);
void add_cell_to_grid(grid* my_grid, int x, int y, cell my_cell);
void add_walls(grid* my_grid);
void random_populate_grid(grid* my_grid, int people);
void is_empty_cell(cell* a_cell, int pos_x, int pos_y); // ?
void move_to(person* a_person, grid* my_grid, int x, int y); // ?
void move_cell(cell* my_cell); //?
*/

#endif // FIELD_H

