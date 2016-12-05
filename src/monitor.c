#include "../inc/monitor.h"
#include <pthread.h>
#include <stdio.h>

void get(struct monitor * ptr_monitor){
    pthread_mutex_lock(&ptr_monitor->mutex_monitor);

    while (ptr_monitor->ticket <= 0)
	pthread_cond_wait(&ptr_monitor->signal_release, &ptr_monitor->mutex_monitor);

    ptr_monitor->ticket--;
    pthread_mutex_unlock(&ptr_monitor->mutex_monitor);
} 

void release(struct monitor * ptr_monitor){
    pthread_mutex_lock(&ptr_monitor->mutex_monitor);
    ptr_monitor->ticket++;
    pthread_cond_signal(&ptr_monitor->signal_release);
    pthread_mutex_unlock(&ptr_monitor->mutex_monitor);
}

void init_monitor(struct monitor * ptr, int ticket){
    pthread_cond_init(&ptr->signal_release, 0);
    pthread_mutex_init(&ptr->mutex_monitor, 0);
    ptr->get = get;
    ptr->release = release;
    ptr->ticket = ticket;
}

void destroy_monitor(struct monitor *ptr_monitor){
    pthread_mutex_destroy(&ptr_monitor->mutex_monitor);
    pthread_cond_destroy(&ptr_monitor->signal_release);
}
