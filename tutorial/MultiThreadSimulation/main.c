#include <pthread.h>
#include <stdio.h>
#include	<stdint.h>
#include <inttypes.h>
#include <unistd.h>

#define GRIDSIZE  5
//THIS PROGRAM TESTS THE DIFFERENT CASES OF THREADS

int grid[GRIDSIZE] = {0,1,2,3,4};

void move(int id)
{
	printf("Moving  %d \n", id);	
}//move()

//================ SINGLE THREAD================//

pthread_t one_thread; //the thread declaration
char decision=' ';


void * one_thread_move()
{
	printf("Moving the elements one by one with one thread\n");
	int i;
	for (i=0; i<GRIDSIZE  ; i++)
	{
		//printf("moving in for");
		move(grid[i]);
	}
	return NULL;
}//one_thread_move()

//=====================================================


//================ FOUR THREADS================//

pthread_t hl_thread; 
pthread_t ll_thread; 
pthread_t hr_thread; 
pthread_t lr_thread; 



void * move_high_left(){printf("Moving High left elements\n");}
void * move_low_left(){printf("Moving low left elements\n");}
void * move_high_right(){printf("Moving High right elements\n");}
void * move_low_right(){printf("Moving low right elements\n");}



void  four_thread_move()
{
	printf("Using a thread for each  four parts of the map\n");
	
	if(pthread_create(&hl_thread, NULL, &move_high_left,NULL))
	{
		
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	if(pthread_join(hl_thread, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}	
	
	
	if(pthread_create(&ll_thread, NULL, &move_low_left,NULL))
	{
		
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	if(pthread_join(ll_thread, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}	
	
	if(pthread_create(&hr_thread, NULL, &move_high_right,NULL))
	{
		
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	if(pthread_join(hr_thread, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}	
	
	
	if(pthread_create(&lr_thread, NULL, &move_low_right,NULL))
	{
		
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	if(pthread_join(lr_thread, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}	
}//four_thread_move()

//=====================================================


//=================ONE THREAD PER ELEMENT======================

pthread_t all_threads[GRIDSIZE];

void * single_element_move()
{
	printf("Moving a single element per Thread \n");
	return NULL;
}//single_element_move()

void  max_thread_move()
{
	printf("Dispatching a thread per element\n");
	for (int i = 0; i < GRIDSIZE; i++)
	{
		if(pthread_create(&all_threads[i], NULL, &single_element_move,NULL))
		{
			
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_join(all_threads[i], NULL)) 
		{
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}	
	}	
}//max_thread_move()


int main()
{
	printf("What type of thread do you want ? \n (0 = 1 general thread,1 = 4 threads (cuz four parts of the map), 2 = a thread per moving element)\n  (0/1/2)");
	scanf("%c",&decision);
	
	if(decision == '0')
	{
		//printf("Decision =0");
		if(pthread_create(&one_thread, NULL, &one_thread_move,NULL))
		{
			
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_join(one_thread, NULL)) 
		{
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}	
	}
	
	if(decision == '1')
	{
		four_thread_move();
	}
	
	if(decision == '2')
	{
		max_thread_move();
	}
	
	printf("In the main program");
	
	return 0;
	
}//main()

