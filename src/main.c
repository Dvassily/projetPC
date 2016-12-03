#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>

#include "../inc/gui.h"
#include "../inc/simulation.h"


int main (int argc, char *argv[])
{
    srand(time(NULL));

    grid field;
    int print_duration = 0;
    scenario scenario = UNKNOWN_SCE;
    unsigned population = 0;
    clock_t timecpu_before = 0;
    time_t timeuser_before = 0;
    
    // Command line parsing
    // TODO :
    // - detect incorrect parameter value (ex : string à la place de int)
    char c;
    while ((c = getopt (argc, argv, "mt:p:")) != -1){
	switch(c){
        case 'm':
            print_duration = 1;
            break;
        case 'p':
	    population = pow(2, atoi(optarg));
            break;
        case 't':
	    scenario = (::scenario) atoi(optarg);
            break;
        default:
            printf("Unknown option : %c\n", c);
            exit(EXIT_FAILURE);
            break;
	}
    }

    if (scenario == UNKNOWN_SCE) {
	fprintf(stderr, "Missing parameter -t\n");
	exit(EXIT_FAILURE);
    }

    if (! population) {
	fprintf(stderr, "Missing parameter -p\n");
	exit(EXIT_FAILURE);	
    }

        
    // Field initialisation
    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
    populate_field(&field, population);

#ifdef GUI
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    if (! init(&window, &renderer)) {
	printf("GUI initialization failed.\n");
	exit(EXIT_FAILURE);
    }
#endif
    
    if (print_duration) {
	timeuser_before = time(NULL);
	timecpu_before = clock();
    }
    
    start_simulation(&field, scenario
#ifdef GUI
		     , renderer
#endif // GUI
		     );
    
    if (print_duration) {
	printf("Execution time : \n");
	printf("CPU : %f\n", (float) (clock() - timecpu_before) / CLOCKS_PER_SEC);
	printf("User : %f\n", (float) (time(NULL) - timeuser_before));
    }

    
#ifdef GUI
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    SDL_Quit();
#endif

    return 0;
}
