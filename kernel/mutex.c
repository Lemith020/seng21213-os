#include "mutex.h"
void mutex_init(mutex_t *m) { m->locked = 0; m->owner_tid = -1; }
void mutex_lock(mutex_t *m) { while(m->locked); m->locked = 1; }
void mutex_unlock(mutex_t *m) { m->locked = 0; }
