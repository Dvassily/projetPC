#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include "../inc/field.h"
#include "../inc/simulation.h"

int main (int argc, char *argv[])
{
    int print_duration = 0;
    scenario scenario = UNKNOWN;
    unsigned population = 0;
    time_t time_before;

    // Command line parsing
    // TODO :
    // - detect incorrect parameter value (ex : string à la place de int)
    char c;
    while ((c = getopt (argc, argv, "mt:p:")) != -1){
	switch(c){
        case 'm':
            time_before = clock();
            print_duration = 1;
            break;
        case 'p':
	    population = pow(2, atoi(optarg));
            break;
        case 't':
            scenario = atoi(optarg);
            break;
        default:
            printf("Unknown option : %c\n", c);
            exit(EXIT_FAILURE);
            break;
	}
    }

    if (scenario == UNKNOWN) {
	fprintf(stderr, "Missing parameter -t\n");
	exit(EXIT_FAILURE);
    }

    if (! population) {
	fprintf(stderr, "Missing parameter -p\n");
	exit(EXIT_FAILURE);	
    }
    
    start_simulation(population, scenario);

    // show execution time of the simulation
    if (print_duration) {
	printf("Execution time : %f\n", (float) (clock() - time_before) / CLOCKS_PER_SEC);
    }

    return 0;
}
