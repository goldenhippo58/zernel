#include "kernel.h"
#include "vga.h"
#include "filesystem.h"
#include "string.h"
#include "memory.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

void handle_command(const char* input) {
    vga_writestring("DEBUG: Entered handle_command\n");
    vga_writestring("Received command: ");
    vga_writestring(input);
    vga_writestring("\n");

    char command[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];
    int arg_count = 0;

    strncpy(command, input, MAX_COMMAND_LENGTH);
    command[MAX_COMMAND_LENGTH - 1] = '\0';

    char* token = strtok(command, " ");
    while (token != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    vga_writestring("Command: ");
    vga_writestring(args[0]);
    vga_writestring("\n");

    if (strcmp(args[0], "help") == 0) {
        vga_writestring("DEBUG: Executing help command\n");
        vga_writestring("Available commands:\n");
        vga_writestring("  help - Display this help message\n");
        vga_writestring("  clear - Clear the screen\n");
        vga_writestring("  create <filename> - Create a new file\n");
        vga_writestring("  write <filename> <content> - Write content to a file\n");
        vga_writestring("  read <filename> - Read content from a file\n");
        vga_writestring("  delete <filename> - Delete a file\n");
        vga_writestring("  list - List all files\n");
        vga_writestring("  mkdir <dirname> - Create a new directory\n");
        vga_writestring("  meminfo - Display memory information\n");
        vga_writestring("  test - Run a series of tests\n");
    } else if (strcmp(args[0], "clear") == 0) {
        vga_writestring("DEBUG: Executing clear command\n");
        vga_writestring("Clearing screen...\n");
        vga_clear();
    } else if (strcmp(args[0], "create") == 0) {
        vga_writestring("DEBUG: Executing create command\n");
        if (arg_count < 2) {
            vga_writestring("Usage: create <filename>\n");
        } else {
            vga_writestring("Attempting to create file: ");
            vga_writestring(args[1]);
            vga_writestring("\n");
            int result = fs_create(args[1]);
            vga_writestring("fs_create result: ");
            char result_str[20];
            int_to_string(result, result_str);
            vga_writestring(result_str);
            vga_writestring("\n");
            if (result >= 0) {
                vga_writestring("File created successfully\n");
            } else if (result == -1) {
                vga_writestring("Error: Maximum number of files reached\n");
            } else if (result == -2) {
                vga_writestring("Error: File already exists\n");
            } else {
                vga_writestring("Error: Failed to create file\n");
            }
        }
    } else if (strcmp(args[0], "write") == 0) {
        vga_writestring("DEBUG: Executing write command\n");
        if (arg_count < 3) {
            vga_writestring("Usage: write <filename> <content>\n");
        } else {
            vga_writestring("Attempting to write to file: ");
            vga_writestring(args[1]);
            vga_writestring("\n");
            int result = fs_write(args[1], args[2], strlen(args[2]));
            vga_writestring("fs_write result: ");
            char result_str[20];
            int_to_string(result, result_str);
            vga_writestring(result_str);
            vga_writestring("\n");
            if (result >= 0) {
                vga_writestring("Content written to file\n");
            } else {
                vga_writestring("Error: Failed to write to file\n");
            }
        }
    } else if (strcmp(args[0], "read") == 0) {
        vga_writestring("DEBUG: Executing read command\n");
        if (arg_count < 2) {
            vga_writestring("Usage: read <filename>\n");
        } else {
            vga_writestring("Attempting to read from file: ");
            vga_writestring(args[1]);
            vga_writestring("\n");
            char buffer[MAX_FILE_SIZE];
            int result = fs_read(args[1], buffer, MAX_FILE_SIZE - 1);
            vga_writestring("fs_read result: ");
            char result_str[20];
            int_to_string(result, result_str);
            vga_writestring(result_str);
            vga_writestring("\n");
            if (result >= 0) {
                buffer[result] = '\0';
                vga_writestring("File contents:\n");
                vga_writestring(buffer);
                vga_writestring("\n");
            } else {
                vga_writestring("Error: Failed to read file\n");
            }
        }
    } else if (strcmp(args[0], "delete") == 0) {
        vga_writestring("DEBUG: Executing delete command\n");
        if (arg_count < 2) {
            vga_writestring("Usage: delete <filename>\n");
        } else {
            vga_writestring("Attempting to delete file: ");
            vga_writestring(args[1]);
            vga_writestring("\n");
            int result = fs_delete(args[1]);
            vga_writestring("fs_delete result: ");
            char result_str[20];
            int_to_string(result, result_str);
            vga_writestring(result_str);
            vga_writestring("\n");
            if (result == 0) {
                vga_writestring("File deleted successfully\n");
            } else {
                vga_writestring("Error: Failed to delete file\n");
            }
        }
    } else if (strcmp(args[0], "list") == 0) {
        vga_writestring("DEBUG: Executing list command\n");
        char buffer[1024];
        fs_list(buffer, sizeof(buffer));
        vga_writestring("Files:\n");
        vga_writestring(buffer);
    } else if (strcmp(args[0], "mkdir") == 0) {
        vga_writestring("DEBUG: Executing mkdir command\n");
        if (arg_count < 2) {
            vga_writestring("Usage: mkdir <dirname>\n");
        } else {
            int result = fs_mkdir(args[1]);
            if (result == 0) {
                vga_writestring("Directory created successfully\n");
            } else {
                vga_writestring("Error: Failed to create directory\n");
            }
        }
    } else if (strcmp(args[0], "meminfo") == 0) {
        vga_writestring("DEBUG: Executing meminfo command\n");
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
        vga_writestring(buffer);
    } else if (strcmp(args[0], "test") == 0) {
        vga_writestring("DEBUG: Executing test command\n");
        vga_writestring("Running tests...\n");
        // Add your test code here
    } else {
        vga_writestring("DEBUG: Unknown command\n");
        vga_writestring("Unknown command. Type 'help' for a list of commands.\n");
    }

    vga_writestring("DEBUG: Exiting handle_command\n");
}
