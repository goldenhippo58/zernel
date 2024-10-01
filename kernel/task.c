#include "task.h"
#include "memory.h"
#include "string.h"
#include "vga.h"

static Task tasks[MAX_TASKS];
static int num_tasks = 0;
static int current_task = -1;

extern void switch_task(uint64_t* old_sp, uint64_t new_sp);

void init_tasking() {
    memset(tasks, 0, sizeof(tasks));
    num_tasks = 0;
    current_task = -1;
}

void create_task(int id, char* name, void (*entry)(void)) {
    if (num_tasks >= MAX_TASKS) {
        vga_writestring("Error: Maximum number of tasks reached\n");
        return;
    }

    Task* task = &tasks[num_tasks];
    task->id = id;
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0';
    task->entry = entry;

    // Allocate stack for the task
    void* stack = kmalloc(STACK_SIZE);
    task->rsp = (uint64_t)stack + STACK_SIZE;

    // Set up initial stack frame
    task->rsp -= sizeof(uint64_t);
    *(uint64_t*)task->rsp = (uint64_t)entry;

    task->cr3 = read_cr3();

    num_tasks++;
}

void schedule() {
    if (num_tasks == 0) return;

    int next_task = (current_task + 1) % num_tasks;
    if (current_task == -1) {
        current_task = next_task;
        tasks[current_task].entry();
    } else {
        Task* old_task = &tasks[current_task];
        Task* new_task = &tasks[next_task];
        current_task = next_task;
        switch_task(&old_task->rsp, new_task->rsp);
    }
}

void yield() {
    schedule();
}
