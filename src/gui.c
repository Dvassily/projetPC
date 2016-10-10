#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "../inc/gui.h"

#define SCALE 2

bool init(SDL_Window** window, SDL_Renderer** renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	printf("Error : Failed to initialize SDL. %s\n", SDL_GetError() );
	return false;
    }

    // Create window
    *window = SDL_CreateWindow("Field view",
			       SDL_WINDOWPOS_UNDEFINED,
			       SDL_WINDOWPOS_UNDEFINED,
			       SCALE * DEFAULT_GRID_WIDTH,
			       SCALE * DEFAULT_GRID_HEIGHT,
			       SDL_WINDOW_SHOWN);
    if (*window == NULL) {
	printf("Window could not be created! SDL Error: %s\n", SDL_GetError() );
	return false;
    }

    // Initialize renderer for window
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED );
    if(*renderer == NULL) {
	    printf("Error : failed to intialize renderer.  %s\n", SDL_GetError() );
	    return false;
    }

    return true;
}

void generate_colors(rgb_color colors[], int number)
{
    for (int i = 0; i < number; ++i) {
	colors[i].red = rand() % 256;
	colors[i].green = rand() % 256;
	colors[i].blue = rand() % 256;
    }
}

void update(SDL_Renderer* renderer, struct grid* field, rgb_color colors[]) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    for (int i = 0; i < field->width; ++i) {
	for (int j = 0; j < field->height; ++j) {
	    rgb_color color;
	    
	    switch(field->matrix[i][j].content) {
	    case EMPTY:
		color.red = 0xFF; color.green = 0xFF; color.blue = 0xFF;
		break;
	    case WALL:
		color.red = 0x00; color.green = 0x00; color.blue = 0x00;
		break;
	    case PERSON:
		//printf("-> (%d %d)\n", i, j);
		for (int p = 0; p < field->person_count; ++p) {
		    //printf("[%d %d]\n", field->people[p].origin_x, field->people[p].origin_y);
		    if ((i >= field->people[p].origin_x && i < field->people[p].origin_x + 4) &&
			(j >= field->people[p].origin_y && j < field->people[p].origin_y + 4)) {
			//printf("foo\n");
			color = colors[p];
		    }
		}
		//printf("%d %d %d\n", color.red, color.green, color.blue);
		
		break;
	    }

	    SDL_Rect rectToDraw = {SCALE * i, SCALE * j, SCALE, SCALE};
	    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 0xFF);
	    SDL_RenderFillRect(renderer, &rectToDraw);	
	}
    }

    SDL_RenderPresent(renderer);
}

int _main(int argc, char *argv[]) {
    srand(time(NULL));
    const unsigned population = 128;
    rgb_color colors[population];
    grid field;
    generate_colors(colors, population);
    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
    populate_field(&field, population);
    
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    if (! init(&window, &renderer)) {
	printf("GUI initialization failed.\n");
	return EXIT_FAILURE;
    }

    SDL_Event e;
    bool running = true;

    while (running) {
	// While there are events in the queue ...
	while (SDL_PollEvent(&e) != 0) {
	    if( e.type == SDL_QUIT ) {
		running = false;
	    }
	}

	// Fill window in white
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	/*
	SDL_Rect rectToDraw = {100,100,100,100};
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &rectToDraw);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderDrawRect(renderer, &rectToDraw);
	*/
	update(renderer, &field, colors);
	
	// Update window
	SDL_RenderPresent(renderer);
    }

    
    return 0;
}

