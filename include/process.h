#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 10
#define STACK_SIZE 4096

typedef enum {
    PROC_UNUSED = 0,
    PROC_READY,
    PROC_RUNNING,
    PROC_TERMINATED
} proc_state_t;

typedef struct {
    int pid;
    proc_state_t state;
    unsigned int esp;
    void (*entry)(void);
    char name[16];
    char stack[STACK_SIZE];
} pcb_t;

extern pcb_t process_table[MAX_PROCESSES];

void process_init(void);
int create_process(void (*entry)(void), const char *name);
void schedule(void);

#endif
