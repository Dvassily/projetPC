#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <algorithm>

#include "../inc/gui.h"
#include "../inc/simulation.h"


int main (int argc, char *argv[])
{
    srand(time(NULL));

    grid field;
    int print_duration = 0;
    scenario scenario = UNKNOWN_SCE;
    unsigned population = 0;
    step step = UNKNOWN_STEP;
    clock_t timecpu_before = 0;
    time_t timeuser_before = 0;

    // Command line parsing
    // TODO :
    // - detect incorrect parameter value (ex : string à la place de int)
    char c;
    while ((c = getopt (argc, argv, "me:t:p:")) != -1){
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
	case 'e':
	    step = (::step) atoi(optarg);
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

    if (step == UNKNOWN_STEP) {
	fprintf(stderr, "Missing parameter -e\n");
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
      std::vector<float> cpu_times_ms;
      std::vector<float> user_times_ms;

      for(int i=0; i<5; ++i){
        timeuser_before = time(NULL);
      	timecpu_before = clock();
        start_simulation(&field, scenario, step
    #ifdef GUI
    		     , renderer
    #endif // GUI
    		     );
        if(i!=4){
	    //free(&field);
	    init_grid(&field, DEFAULT_GRID_WIDTH, DEFAULT_GRID_HEIGHT);
	    populate_field(&field, population);
        }
        cpu_times_ms.push_back((float) (clock() - timecpu_before) * 1000 / CLOCKS_PER_SEC);
        user_times_ms.push_back((float) (time(NULL) - timeuser_before) * 1000);
      }

      float cpu_max = *max_element(cpu_times_ms.begin(), cpu_times_ms.end());
      float cpu_min = *min_element(cpu_times_ms.begin(), cpu_times_ms.end());
      float user_max = *max_element(user_times_ms.begin(), user_times_ms.end());
      float user_min = *min_element(user_times_ms.begin(), user_times_ms.end());

      cpu_times_ms.erase(remove(cpu_times_ms.begin(), cpu_times_ms.end(), cpu_max), cpu_times_ms.end());
      cpu_times_ms.erase(remove(cpu_times_ms.begin(), cpu_times_ms.end(), cpu_min), cpu_times_ms.end());
      user_times_ms.erase(remove(user_times_ms.begin(), user_times_ms.end(), user_max), user_times_ms.end());
      user_times_ms.erase(remove(user_times_ms.begin(), user_times_ms.end(), user_min), user_times_ms.end());

      float cpu_moy, user_moy;
      for (unsigned i = 0; i < cpu_times_ms.size(); i++) cpu_moy += cpu_times_ms[i];
      cpu_moy = cpu_moy/ (int) cpu_times_ms.size();
      for (unsigned i = 0; i < user_times_ms.size(); i++) user_moy += user_times_ms[i];
      user_moy = user_moy/ (int) user_times_ms.size();

      printf("Execution Time (in ms) : \n CPU : %.3f ms\n USER : %.3f ms", cpu_moy, user_moy);
    }
    else {
      start_simulation(&field, scenario, step
  #ifdef GUI
  		     , renderer
  #endif // GUI
  		     );
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
