
# ML Kernel Project

This project is a custom kernel implementation developed as a learning exercise. It includes basic functionalities such as memory management, a simple file system, task scheduling, and a command-line interface.

## Project Structure

```
ml-kernel/
├── bootloader/
│   ├── Makefile
│   ├── boot.asm
│   ├── linker.ld
│   └── loaderer.c
├── build/
│   └── kernel.bin
├── include/
│   ├── filesystem.h
│   ├── gpu.h
│   ├── interrupt.h
│   ├── io.h
│   ├── kernel.h
│   ├── keyboard.h
│   ├── memory.h
│   ├── process.h
│   ├── serial.h
│   ├── string.h
│   ├── syscall.h
│   ├── task.h
│   ├── timer.h
│   └── vga.h
├── iso/
│   ├── boot/
│   │   ├── grub/
│   │   └── kernel.bin
│   └── grub/
│       └── grub.cfg
├── kernel/
│   ├── Makefile
│   ├── drivers/
│   │   ├── gpu.c
│   │   ├── keyboard.c
│   │   ├── serial.c
│   │   ├── timer.c
│   │   └── vga.c
│   ├── filesystem.c
│   ├── interrupt.c
│   ├── kernel.c
│   ├── kernel_helpers.c
│   ├── linker.ld
│   ├── memory.c
│   ├── process.c
│   ├── string.c
│   ├── syscall.c
│   └── task.c
├── tools/
│   ├── create-iso.sh
│   └── qemu-run.sh
└── README.md
```

## Features

- Basic memory management (physical and virtual memory allocation)
- Simple in-memory file system
- Task scheduling
- VGA text mode output
- Keyboard input
- Serial port logging
- Command-line interface with basic commands

## Building the Kernel

To build the kernel, follow these steps:

1. Navigate to the kernel directory:
   ```
   cd ml-kernel/kernel
   ```

2. Run the make command:
   ```
   make clean
   make
   ```

3. If successful, this will create a `kernel.bin` file in the `build` directory.

## Creating a Bootable ISO

To create a bootable ISO:

1. Copy the kernel binary to the ISO directory:
   ```
   cp build/kernel.bin iso/boot/
   ```

2. Use GRUB to create the ISO:
   ```
   grub-mkrescue -o mykernel.iso iso
   ```

## Running the Kernel

To run the kernel in QEMU:

```
qemu-system-x86_64 -cdrom mykernel.iso
```

## Available Commands

Once the kernel is running, you can use the following commands:

- `help`: Display a list of available commands
- `clear`: Clear the screen
- `create <filename>`: Create a new file
- `write <filename> <content>`: Write content to a file
- `read <filename>`: Read content from a file
- `delete <filename>`: Delete a file
- `list`: List all files
- `meminfo`: Display memory information
- `test`: Run a series of tests (if implemented)

## Debugging

The kernel includes various debug statements throughout the code. These can be enabled or modified to help diagnose issues during development.

## Future Improvements

- Implement a more sophisticated file system
- Add support for running user-space programs
- Improve memory management with paging and virtual memory
- Implement more system calls
- Add networking capabilities

## Contributing

This is a personal learning project, but suggestions and improvements are welcome. Please open an issue or submit a pull request if you have any contributions.

## License

>>>>>>> f94e0d043def3ae826f53a174132f1f326baf6e3
