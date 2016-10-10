#include <pthread.h>
#include <stdio.h>
#include	<stdint.h>
#include <inttypes.h>
#include <unistd.h>


//THIS PROGRAM TESTS THE DIFFERENCE BETWEEN JOINING A THREAD AND NOT JOINING IT


//function will be executed by the thread
void *in_thread_function()
{
	printf("In the thread !!\n");	
	sleep(1);
	return NULL; //function must return something
}


int main()
{
	pthread_t my_thread; //the thread declaration
	char decision=' ';
	
	printf("Do you want to join the thread ? (y/n)");
	scanf("%c",&decision);
	
	//Using if for the creation, because pthread_create returns 0 if success, 1 if failure
	if(pthread_create(&my_thread, NULL, in_thread_function,NULL))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	
	if(decision == 'y')
	{
		//Joining the thread, which means that the program has to wait for my_thread execution to finish before ending the main thread
		if(pthread_join(my_thread, NULL)) //Same principle as above
		{
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}
	}
	
	//sleep(2); //Uncomment this one if you want to see the thread executing without joining
	printf("In the main program");
	
	return 0;
	
}//main()

