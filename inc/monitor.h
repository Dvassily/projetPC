#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <iostream>
#include <string>
#include <pthread.h>

typedef struct monitor {
    void (*get)(struct monitor *);
    void (*release)(struct monitor*);
    pthread_mutex_t mutex_monitor;
    pthread_cond_t signal_release;
    int ticket;
} monitor;

void init_monitor(struct monitor * ptr_monitor, int ticket);
void destroy_monitor(struct monitor *ptr_monitor);
#endif // __MONITOR_H__
