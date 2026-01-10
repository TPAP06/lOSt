// kernel/drivers/keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Special key codes (use unsigned char/uint8_t to hold values > 127)
#define KEY_UP_ARROW    ((unsigned char)0x80)
#define KEY_DOWN_ARROW  ((unsigned char)0x81)
#define KEY_LEFT_ARROW  ((unsigned char)0x82)
#define KEY_RIGHT_ARROW ((unsigned char)0x83)
#define KEY_HOME        ((unsigned char)0x84)
#define KEY_END         ((unsigned char)0x85)
#define KEY_DELETE      ((unsigned char)0x86)
#define KEY_PAGE_UP     ((unsigned char)0x87)
#define KEY_PAGE_DOWN   ((unsigned char)0x88)

// Initialize the keyboard driver
void keyboard_init(void);

// Check if a key is available
bool keyboard_available(void);

// Get a character from the keyboard (blocking)
// Returns unsigned char to support special keys > 127
unsigned char keyboard_getchar(void);

// Read a line from keyboard into buffer (blocking)
void keyboard_readline(char *buffer, int max_length);

// Read a line with history support
void keyboard_readline_history(char *buffer, int max_length, 
                                char history[][256], int history_max, 
                                int history_count, int *history_write_pos);

// Keyboard state (read-only access)
extern bool ctrl_pressed;

#endif // KEYBOARD_H