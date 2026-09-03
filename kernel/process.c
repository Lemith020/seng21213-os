#include "process.h"
#include "types.h"

pcb_t process_table[MAX_PROCESSES];
static int next_pid = 0;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROC_UNUSED;
    }
}

int create_process(void (*entry)(void), const char *name) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROC_UNUSED) {
            pcb_t *p = &process_table[i];
            p->pid = next_pid++;
            p->state = PROC_READY;
            p->entry = entry;
            
            int j = 0;
            while (name[j] != '\0' && j < 15) {
                p->name[j] = name[j];
                j++;
            }
            p->name[j] = '\0';

            unsigned int *stack_top = (unsigned int *)(p->stack + STACK_SIZE);
            *(--stack_top) = (unsigned int)entry;
            p->esp = (unsigned int)stack_top;
            return p->pid;
        }
    }
    return -1;
}
