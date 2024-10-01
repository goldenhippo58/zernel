# Compiler and assembler
AS=nasm
CC=x86_64-elf-gcc
LD=x86_64-elf-ld

# Flags
ASFLAGS=-f elf64
CFLAGS=-ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
LDFLAGS=-n -T linker.ld

# Source files
ASM_SOURCES=boot.asm long_mode_start.asm
C_SOURCES=loaderer.c

# Object files
OBJECTS=$(ASM_SOURCES:.asm=.o) $(C_SOURCES:.c=.o)

# Output binary
OUTPUT=../build/kernel.bin

# Default target
all: $(OUTPUT)

# Link object files into kernel binary
$(OUTPUT): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile assembly files
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build files
clean:
	rm -f $(OBJECTS) $(OUTPUT)

.PHONY: all clean
