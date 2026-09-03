#include "process.h"

extern void context_switch(unsigned int *old_esp, unsigned int new_esp);

static int current_process = -1;

void schedule(void) {
    int prev = current_process;
    int start = (current_process + 1) % MAX_PROCESSES;
    int i = start;

    do {
        if (process_table[i].state == PROC_READY ||
            process_table[i].state == PROC_RUNNING) {
            current_process = i;
            break;
        }
        i = (i + 1) % MAX_PROCESSES;
    } while (i != start);

    if (prev != current_process) {
        if (prev != -1 && process_table[prev].state == PROC_RUNNING) {
            process_table[prev].state = PROC_READY;
        }
        process_table[current_process].state = PROC_RUNNING;
        
        unsigned int *old_esp = (prev != -1) ? &process_table[prev].esp : 0;
        context_switch(old_esp, process_table[current_process].esp);
    }
}
