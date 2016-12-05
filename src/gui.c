#ifdef GUI
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "../inc/gui.h"

#define SCALE 2

void* display_thread_function(void* ptr_args) {
    grid* field = ((display_thread_args*) ptr_args)->field;
    //SDL_Renderer* renderer = ((display_thread_args*) ptr_args)->renderer;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    if (! init(&window, &renderer)) {
	printf("GUI initialization failed.\n");
	exit(EXIT_FAILURE);
    }

    
    while (! is_finished(field)) {
	SDL_Event e;
	/*
	while (SDL_PollEvent(&e) != 0) {
	    if( e.type == SDL_QUIT ) {
		for (unsigned i = 0; i < NB_ZONE; ++i) {
		    pthread_cancel(thread[i]);
		}
		return;
	    }
	}
	*/
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	update(renderer, field);
	SDL_RenderPresent(renderer);
    }

    return NULL;
}

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

void update(SDL_Renderer* renderer, struct grid* field) {
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
		for (unsigned p = 0; p < field->person_count; ++p) {
		    if ((i >= field->people[p].origin_x && i < field->people[p].origin_x + 4) &&
			(j >= field->people[p].origin_y && j < field->people[p].origin_y + 4)) {
			color.red = field->people[p].red_color;
			color.green = field->people[p].green_color;
			color.blue = field->people[p].blue_color;
		    }
		}

		break;
	    }

	    SDL_Rect rectToDraw = {SCALE * i, SCALE * j, SCALE, SCALE};
	    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 0xFF);
	    SDL_RenderFillRect(renderer, &rectToDraw);
	}
    }

    SDL_RenderPresent(renderer);
}

#endif // GUI
