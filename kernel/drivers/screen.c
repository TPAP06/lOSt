// kernel/drivers/screen.c - VGA text mode screen driver implementation

#include "screen.h"

#define VGA_MEMORY 0xB8000

// VGA buffer as array of 16-bit values
static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;

// Current cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Current color attribute
static uint8_t current_color = 0x0F; // White on black

// Create a VGA color attribute byte
uint8_t vga_color_byte(vga_color fg, vga_color bg)
{
    return (bg << 4) | (fg & 0x0F);
}

// Initialize the screen driver
void screen_init(void)
{
    cursor_x = 0;
    cursor_y = 0;
    current_color = vga_color_byte(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_clear();
}

// Clear the screen
void screen_clear(void)
{
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    cursor_x = 0;
    cursor_y = 0;
}

// Scroll the screen up by one line
void screen_scroll(void)
{
    uint16_t blank = ' ' | (current_color << 8);
    
    // Move all lines up by one
    for (int i = 0; i < SCREEN_WIDTH * (SCREEN_HEIGHT - 1); i++) {
        vga_buffer[i] = vga_buffer[i + SCREEN_WIDTH];
    }
    
    // Clear the last line
    for (int i = SCREEN_WIDTH * (SCREEN_HEIGHT - 1); i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    cursor_y = SCREEN_HEIGHT - 1;
}

// Write a single character
void screen_putchar(char c)
{
    // Handle special characters
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1); // Align to next 4-space tab stop
    } else if (c == '\b') {// Backspace
        if (cursor_x > 0) {
            cursor_x--;
            int offset = cursor_y * SCREEN_WIDTH + cursor_x;
            vga_buffer[offset] = ' ' | (current_color << 8);
        }
    } else {
        // Regular character
        int offset = cursor_y * SCREEN_WIDTH + cursor_x;
        vga_buffer[offset] = c | (current_color << 8);
        cursor_x++;
    }
    
    // Handle line wrap
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    // Handle scroll
    if (cursor_y >= SCREEN_HEIGHT) {
        screen_scroll();
    }
}

// Write a string with default color
void screen_write(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        screen_putchar(str[i]);
    }
}

// Write a string with specified color
void screen_write_color(const char *str, vga_color fg, vga_color bg)
{
    uint8_t old_color = current_color;
    current_color = vga_color_byte(fg, bg);
    screen_write(str);
    current_color = old_color;
}

// Set text color for subsequent writes
void screen_set_color(vga_color fg, vga_color bg)
{
    current_color = vga_color_byte(fg, bg);
}

// Get cursor position
void screen_get_cursor(int *x, int *y)
{
    if (x) *x = cursor_x;
    if (y) *y = cursor_y;
}

// Set cursor position
void screen_set_cursor(int x, int y)
{
    if (x >= 0 && x < SCREEN_WIDTH) {
        cursor_x = x;
    }
    if (y >= 0 && y < SCREEN_HEIGHT) {
        cursor_y = y;
    }
}