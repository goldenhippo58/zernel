OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_main)

SECTIONS
{
    /* Set the start address of the kernel */
    . = 1M;

    .multiboot_header ALIGN(4K) : {
        *(.multiboot_header)
    }

    .text ALIGN(4K) : {
        *(.text*)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data*)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
