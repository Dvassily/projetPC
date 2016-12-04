#include <pthread>

typedef struct {
  void (*get)(struct monitor *);
  void (*release)(struct monitor*);
  pthread_mutex_t mutex_monitor;
} monitor;

void init_monitor(struct monitor * ptr_monitor);
void destroy_monitor(struct monitor *ptr_monitor);
