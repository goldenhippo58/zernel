# Compiler and flags
CC = x86_64-linux-gnu-gcc
AS = nasm
CFLAGS = -ffreestanding -m64 -mcmodel=large -fno-asynchronous-unwind-tables -fno-pic -O0 -g -Wall -Wextra -I../include
ASFLAGS = -f elf64
LDFLAGS = -ffreestanding -m64 -nostdlib -nodefaultlibs -no-pie -T linker.ld

# Source files
SOURCES = kernel.c kernel_helpers.c interrupt.c memory.c process.c syscall.c filesystem.c string.c task.c
DRIVER_SOURCES = drivers/serial.c drivers/vga.c drivers/timer.c drivers/keyboard.c
ASM_SOURCES = boot.asm long_mode_start.asm task_switch.asm
OBJECTS = $(SOURCES:.c=.o) $(DRIVER_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

# Output binary
OUTPUT = ../build/kernel.bin

# Default target
all: $(OUTPUT)

# Link objects into the kernel binary
$(OUTPUT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM source files into object files
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(OUTPUT)

.PHONY: all clean
