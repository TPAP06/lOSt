// kernel/drivers/screen.h

#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Scrollback buffer size (lines)
#define SCROLLBACK_LINES 1000

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

// Initialize screen
void screen_init(void);

// Initialize scrollback buffer (call AFTER heap_init)
void screen_init_scrollback(void);

// Write functions
void screen_write(const char* str);
void screen_write_color(const char* str, vga_color fg, vga_color bg);
void screen_putchar(char c);

// Clear screen
void screen_clear(void);

// Color management
void screen_set_color(vga_color fg, vga_color bg);
uint8_t vga_color_byte(vga_color fg, vga_color bg);

// Cursor functions
void screen_get_cursor(int *x, int *y);
void screen_set_cursor(int x, int y);

// Scrollback functions
void screen_scroll_up(void);     // Page Up
void screen_scroll_down(void);   // Page Down
void screen_scroll_to_bottom(void);
bool screen_is_at_bottom(void);

#endif // SCREEN_H