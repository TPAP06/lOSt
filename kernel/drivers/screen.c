// kernel/drivers/screen.c - Complete scrollback implementation

#include "screen.h"
#include "../lib/string.h"
#include "../memory/heap.h"

// VGA buffer
static volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Current color
static uint8_t current_color;

// Scrollback buffer (circular buffer of lines)
#define BUFFER_LINES 200
static uint16_t line_buffer[BUFFER_LINES][SCREEN_WIDTH];
static int buffer_start = 0;        // Oldest line in buffer
static int buffer_count = 0;        // Number of lines in buffer
static int scroll_offset = 0;       // How many lines scrolled up from bottom
static bool scrollback_active = false;

// Current screen content (what should be visible at scroll_offset=0)
static uint16_t current_screen[SCREEN_HEIGHT][SCREEN_WIDTH];

// Create VGA color byte
uint8_t vga_color_byte(vga_color fg, vga_color bg)
{
    return fg | (bg << 4);
}

// Initialize screen (early)
void screen_init(void)
{
    cursor_x = 0;
    cursor_y = 0;
    current_color = vga_color_byte(COLOR_LIGHT_GREY, COLOR_BLACK);
    scrollback_active = false;
    buffer_count = 0;
    buffer_start = 0;
    scroll_offset = 0;
    screen_clear();
}

// Initialize scrollback (after heap)
void screen_init_scrollback(void)
{
    memset(line_buffer, 0, sizeof(line_buffer));
    memset(current_screen, 0, sizeof(current_screen));
    scrollback_active = true;
}

// Clear screen
void screen_clear(void)
{
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    if (scrollback_active) {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                current_screen[y][x] = blank;
            }
        }
    }
    
    cursor_x = 0;
    cursor_y = 0;
    scroll_offset = 0;
}

// Add a line to scrollback buffer
static void add_line_to_buffer(uint16_t *line)
{
    if (!scrollback_active) return;
    
    int write_pos = (buffer_start + buffer_count) % BUFFER_LINES;
    
    // Copy line to buffer
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        line_buffer[write_pos][x] = line[x];
    }
    
    if (buffer_count < BUFFER_LINES) {
        buffer_count++;
    } else {
        // Buffer full, overwrite oldest
        buffer_start = (buffer_start + 1) % BUFFER_LINES;
    }
}

// Refresh VGA display based on scroll offset
static void refresh_display(void)
{
    if (!scrollback_active || scroll_offset == 0) {
        // Show current screen
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                vga_buffer[y * SCREEN_WIDTH + x] = current_screen[y][x];
            }
        }
    } else {
        // Show scrolled view
        // Calculate which lines to display
        int total_lines = buffer_count + SCREEN_HEIGHT;
        int top_line = total_lines - SCREEN_HEIGHT - scroll_offset;
        
        if (top_line < 0) top_line = 0;
        
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            int source_line = top_line + y;
            
            if (source_line < buffer_count) {
                // From scrollback buffer
                int buffer_idx = (buffer_start + source_line) % BUFFER_LINES;
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    vga_buffer[y * SCREEN_WIDTH + x] = line_buffer[buffer_idx][x];
                }
            } else {
                // From current screen
                int screen_line = source_line - buffer_count;
                if (screen_line >= 0 && screen_line < SCREEN_HEIGHT) {
                    for (int x = 0; x < SCREEN_WIDTH; x++) {
                        vga_buffer[y * SCREEN_WIDTH + x] = current_screen[screen_line][x];
                    }
                }
            }
        }
        
        // Show scroll indicator in top-right corner
        vga_buffer[79] = '^' | 0x0E00;  // Yellow up arrow
    }
}

// Scroll screen up by one line
static void scroll_screen(void)
{
    uint16_t blank = ' ' | (current_color << 8);
    
    // Save top line to scrollback buffer
    if (scrollback_active) {
        add_line_to_buffer(current_screen[0]);
    }
    
    // Move lines up in current_screen
    for (int y = 0; y < SCREEN_HEIGHT - 1; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            current_screen[y][x] = current_screen[y + 1][x];
        }
    }
    
    // Clear last line
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        current_screen[SCREEN_HEIGHT - 1][x] = blank;
    }
    
    cursor_y = SCREEN_HEIGHT - 1;
    
    // Refresh VGA if we're at bottom
    if (scroll_offset == 0) {
        refresh_display();
    }
}

// Put character on screen
void screen_putchar(char c)
{
    // If scrolled up, jump to bottom on new input
    if (scroll_offset > 0) {
        scroll_offset = 0;
        refresh_display();
    }
    
    uint16_t attribute = current_color << 8;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            current_screen[cursor_y][cursor_x] = ' ' | attribute;
            if (scroll_offset == 0) {
                vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = ' ' | attribute;
            }
        }
    } else {
        current_screen[cursor_y][cursor_x] = c | attribute;
        if (scroll_offset == 0) {
            vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = c | attribute;
        }
        cursor_x++;
    }
    
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= SCREEN_HEIGHT) {
        scroll_screen();
    }
}

// Write string
void screen_write(const char* str)
{
    while (*str) {
        screen_putchar(*str++);
    }
}

// Write string with color
void screen_write_color(const char* str, vga_color fg, vga_color bg)
{
    uint8_t old_color = current_color;
    current_color = vga_color_byte(fg, bg);
    screen_write(str);
    current_color = old_color;
}

// Set color
void screen_set_color(vga_color fg, vga_color bg)
{
    current_color = vga_color_byte(fg, bg);
}

// Get cursor
void screen_get_cursor(int *x, int *y)
{
    if (x) *x = cursor_x;
    if (y) *y = cursor_y;
}

// Set cursor
void screen_set_cursor(int x, int y)
{
    if (x >= 0 && x < SCREEN_WIDTH) cursor_x = x;
    if (y >= 0 && y < SCREEN_HEIGHT) cursor_y = y;
}

// Scroll up (show older content)
void screen_scroll_up(void)
{
    if (!scrollback_active) return;
    
    // Maximum scroll is total lines minus screen height
    int max_scroll = buffer_count;
    
    if (scroll_offset < max_scroll) {
        scroll_offset += 5;  // Scroll by 5 lines at a time
        if (scroll_offset > max_scroll) {
            scroll_offset = max_scroll;
        }
        refresh_display();
    }
}

// Scroll down (show newer content)
void screen_scroll_down(void)
{
    if (!scrollback_active) return;
    
    if (scroll_offset > 0) {
        scroll_offset -= 5;  // Scroll by 5 lines at a time
        if (scroll_offset < 0) {
            scroll_offset = 0;
        }
        refresh_display();
    }
}

// Jump to bottom
void screen_scroll_to_bottom(void)
{
    if (scroll_offset > 0) {
        scroll_offset = 0;
        refresh_display();
    }
}

// Check if at bottom
bool screen_is_at_bottom(void)
{
    return scroll_offset == 0;
}