// kernel/drivers/screen.h - VGA text mode screen driver

#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

// VGA color codes
typedef enum {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
} vga_color;

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Initialize the screen driver
void screen_init(void);

// Clear the screen
void screen_clear(void);

// Write a single character
void screen_putchar(char c);

// Write a string with default color
void screen_write(const char *str);

// Write a string with specified color
void screen_write_color(const char *str, vga_color fg, vga_color bg);

// Set text color for subsequent writes
void screen_set_color(vga_color fg, vga_color bg);

// Get cursor position
void screen_get_cursor(int *x, int *y);

// Set cursor position
void screen_set_cursor(int x, int y);

// Scroll the screen up by one line
void screen_scroll(void);

// Create a color attribute byte
uint8_t vga_color_byte(vga_color fg, vga_color bg);

#endif // SCREEN_H