// kernel/drivers/keyboard.c - Full keyboard driver

#include "keyboard.h"
#include "screen.h"
#include "../interrupts/isr.h"
#include "../lib/io.h"
#include "../lib/string.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Circular buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_start = 0;
static volatile int buffer_end = 0;

// Keyboard state
static bool shift_pressed = false;
static bool caps_lock = false;
static bool ctrl_pressed = false;

// US QWERTY scancode to ASCII
static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Add character to buffer
static void buffer_add(char c)
{
    int next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

// Get character from buffer
static char buffer_get(void)
{
    if (buffer_start == buffer_end) {
        return 0;
    }
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Keyboard interrupt handler
static void keyboard_handler(registers_t *regs)
{
    (void)regs;
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle key release (high bit set)
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        // Check for shift release
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = false;
        }
        // Check for ctrl release
        else if (scancode == 0x1D) {
            ctrl_pressed = false;
        }
        return;
    }
    
    // Handle special keys
    if (scancode == 0x2A || scancode == 0x36) {
        // Left or right shift pressed
        shift_pressed = true;
        return;
    }
    
    if (scancode == 0x1D) {
        // Ctrl pressed
        ctrl_pressed = true;
        return;
    }
    
    if (scancode == 0x3A) {
        // Caps lock toggle
        caps_lock = !caps_lock;
        return;
    }
    
    // Convert scancode to ASCII
    char c = 0;
    
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            c = scancode_to_ascii_shift[scancode];
        } else {
            c = scancode_to_ascii[scancode];
        }
        
        // Apply caps lock to letters only
        if (caps_lock) {
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            } else if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
        }
    }
    
    // Add to buffer if valid
    if (c != 0) {
        buffer_add(c);
    }
}

// Initialize keyboard driver
void keyboard_init(void)
{
    buffer_start = 0;
    buffer_end = 0;
    shift_pressed = false;
    caps_lock = false;
    ctrl_pressed = false;
    
    // Install keyboard IRQ handler (IRQ1)
    irq_install_handler(1, keyboard_handler);
}

// Check if a key is available
bool keyboard_available(void)
{
    return buffer_start != buffer_end;
}

// Get a character (blocking)
char keyboard_getchar(void)
{
    while (!keyboard_available()) {
        __asm__ volatile ("hlt");
    }
    return buffer_get();
}

// Read a line (blocking)
void keyboard_readline(char *buffer, int max_length)
{
    int pos = 0;
    
    while (pos < max_length - 1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            screen_putchar('\n');
            break;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                screen_putchar('\b');
            }
        } else {
            buffer[pos] = c;
            pos++;
            screen_putchar(c);
        }
    }
    
    buffer[pos] = '\0';
}