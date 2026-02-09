// kernel/drivers/screen.h

#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stdbool.h>

#include "../lib/string.h"  // For types (size_t)

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Scrollback buffer size (lines)
#define SCROLLBACK_LINES 200

// Scrollback
typedef struct{
    uint16_t *buffer;       // sb buffer
    uint16_t view_top;      // idx of fullscreen live window (W*H) on sb
    uint16_t count;         // Number of rows inside the buffer
    uint16_t scroll_offset; // Distance from bottom in rows
    uint16_t start;         // Oldest line inside the buffer
    bool follow_bottom;     // 
}sb_t;

#define LEFT_MARGIN 2

// VGA colors
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
    COLOR_WHITE = 15,
} vga_color;

// Clears VGA
void vga_clear(void);

// Writing functions
void vga_refresh(const uint16_t *fullscreen, size_t size);
void vga_set_row(const uint16_t *row, uint8_t idx);
void vga_putchar(uint8_t x, uint8_t y, uint16_t c, uint16_t attr);

// Cursor functions
void vga_set_cursor(uint8_t x, uint8_t y);
void vga_enable_cursor(bool flag);

#endif // SCREEN_H