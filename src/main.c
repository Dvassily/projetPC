#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include "../inc/simulation.h"

int main (int argc, char *argv[])
{
    srand(time(NULL));

    int print_duration = 0;
    scenario scenario = UNKNOWN_SCE;
    unsigned population = 0;

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
    
    start_simulation(population, scenario, print_duration);

    return 0;
}
