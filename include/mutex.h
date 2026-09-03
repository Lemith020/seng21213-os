#ifndef MUTEX_H
#define MUTEX_H
typedef struct { int locked; int owner_tid; } mutex_t;
void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);
#endif
