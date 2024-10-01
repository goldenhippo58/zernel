#include "kernel.h"
#include "vga.h"
#include "serial.h"
#include "keyboard.h"
#include "string.h"
#include "task.h"
#include "memory.h"
#include "filesystem.h"

#define TOTAL_MEMORY_SIZE (1024 * 1024 * 1024) // Assume 1GB of RAM

void log_message(const char *message)
{
    vga_write(message);    // Write to VGA console
    serial_write(message); // Write to serial port
}

void print_memory_info() {
    MemoryInfo info;
    get_memory_info(&info);
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Memory Info:\n"
             "  Total: %llu bytes\n"
             "  Free:  %llu bytes\n"
             "  Used:  %llu bytes\n"
             "  Reserved: %llu bytes\n",
             info.total_memory, info.free_memory,
             info.used_memory, info.reserved_memory);
    log_message(buffer);
}

void task1() {
    while (1) {
        log_message("Task 1 running\n");
        for (volatile int i = 0; i < 1000000; i++) {} // Delay
        yield();
    }
}

void task2() {
    while (1) {
        log_message("Task 2 running\n");
        for (volatile int i = 0; i < 1000000; i++) {} // Delay
        yield();
    }
}

void kernel_main()
{
    vga_init();    // Initialize VGA for CLI output
    serial_init(); // Initialize serial port for logging
    keyboard_init(); // Initialize keyboard

    log_message("DEBUG: Kernel main started\n");

    log_message("Initializing memory management...\n");
    init_physical_memory(TOTAL_MEMORY_SIZE);
    init_virtual_memory();
    init_heap();

    log_message("Initializing file system...\n");
    fs_init();
    log_message("File system initialized.\n");

    // Test file system
    log_message("Testing file system...\n");
    if (fs_create("test.txt") >= 0) {
        log_message("Created test.txt\n");
        if (fs_write("test.txt", "Hello, World!", 13) >= 0) {
            log_message("Wrote to test.txt\n");
            char buffer[20];
            if (fs_read("test.txt", buffer, 20) >= 0) {
                log_message("Read from test.txt: ");
                log_message(buffer);
                log_message("\n");
            } else {
                log_message("Failed to read from test.txt\n");
            }
        } else {
            log_message("Failed to write to test.txt\n");
        }
    } else {
        log_message("Failed to create test.txt\n");
    }

    init_tasking(); // Initialize task scheduler

    log_message("Welcome to ML Kernel\n");
    log_message("Type 'help' for a list of commands\n");

    // Print initial memory info
    print_memory_info();

    create_task(1, "Task 1", task1);
    create_task(2, "Task 2", task2);

    log_message("DEBUG: Entering main loop\n");

    int loop_count = 0;
    while (1)
    {
        char debug_msg[50];
        snprintf(debug_msg, sizeof(debug_msg), "DEBUG: Loop iteration %d\n", loop_count++);
        log_message(debug_msg);

        char input[128];
        log_message("> ");
        log_message("DEBUG: About to read input\n");
        read_input(input);
        log_message("DEBUG: Input read\n");
        log_message("You entered: ");
        log_message(input);
        log_message("\n");
        log_message("DEBUG: About to handle command\n");
        handle_command(input);
        log_message("DEBUG: Command handled\n");

        schedule(); // Run tasks

        // Add a small delay
        for (volatile int i = 0; i < 1000000; i++) {}
    }
}
