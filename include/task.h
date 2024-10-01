#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_TASKS 10
#define STACK_SIZE 4096

typedef struct {
    uint64_t rsp;  // Stack pointer
    uint64_t cr3;  // Page table base register
    void (*entry)(void);  // Entry point of the task
    int id;
    char name[32];
} Task;

void init_tasking();
void create_task(int id, char* name, void (*entry)(void));
void schedule();
void yield();

#endif // TASK_H
