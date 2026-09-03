#ifndef THREAD_H
#define THREAD_H
#include "process.h"
#define MAX_THREADS 16
typedef struct { int tid; proc_state_t state; unsigned int esp; void (*entry)(void *arg); void *arg; char name[16]; char stack[512]; } tcb_t;
extern tcb_t thread_table[MAX_THREADS];
void thread_init(void);
int thread_create(void (*entry)(void *arg), void *arg, const char *name);
#endif
