#include "keyboard.h"
#include "io.h"
#include "vga.h"
#include "string.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define ENTER_KEY_CODE 0x1C

static char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void keyboard_init() {
    // Keyboard initialization code (if needed)
}

char keyboard_read_char() {
    if (inb(KEYBOARD_STATUS_PORT) & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        if (!(scancode & 0x80)) {
            if (scancode == ENTER_KEY_CODE) {
                return '\n';
            }
            return keyboard_map[scancode];
        }
    }
    return 0;
}

void read_input(char* buffer) {
    int i = 0;
    char c;
    vga_writestring("Input: ");
    while (1) {
        c = keyboard_read_char();
        if (c == '\n') {
            buffer[i] = '\0';
            vga_putchar('\n');
            vga_writestring("Finished input: ");
            vga_writestring(buffer);
            vga_putchar('\n');
            return;
        } else if (c != 0) {
            if (c == '\b' && i > 0) {
                i--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            } else if (c >= ' ' && c <= '~' && i < 127) {
                buffer[i++] = c;
                vga_putchar(c);
            }
        }
        // Add a small delay to prevent busy-waiting
        for (volatile int j = 0; j < 10000; j++) {}
    }
}
