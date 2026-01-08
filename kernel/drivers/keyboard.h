// kernel/drivers/keyboard.h - PS/2 Keyboard driver

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Initialize the keyboard driver
void keyboard_init(void);

// Check if a key is available
bool keyboard_available(void);

// Get a character from the keyboard (blocking)
char keyboard_getchar(void);

// Read a line from keyboard into buffer (blocking)
void keyboard_readline(char *buffer, int max_length);

#endif // KEYBOARD_H