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
static uint16_t current_attribute;

// Scrollback buffer (circular buffer of lines)
#define BUFFER_LINES 200
static uint16_t line_buffer[BUFFER_LINES][SCREEN_WIDTH];
static int buffer_start = 0;        // Oldest line in buffer
static int buffer_count = 0;        // Number of lines in buffer
static int scroll_offset = 0;       // How many lines scrolled up from bottom
static bool follow_bottom = true; //

// Holds the possition of the last char in each
static uint8_t line_len[BUFFER_LINES];

// Current screen content (what should be visible at scroll_offset=0)
// It's just easier this way, i wont be bothered optimizing this out yet
static uint16_t current_screen[SCREEN_HEIGHT][SCREEN_WIDTH];

// Create VGA attribute byte
uint16_t vga_attribute_byte(vga_color fg, vga_color bg)
{
    return (fg | (bg << 4)) << 8;
}

// Initialize screen (early)
void screen_init(void)
{
    cursor_x = 0;
    cursor_y = 0;
    current_attribute = 0x0700; // Light-Grey fg and Black bg. Otherwise use vga_attribute_byte
    follow_bottom = true;
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
    follow_bottom = false;
}

// Clear screen
void screen_clear(void)
{
    uint16_t blank = ' ' | current_attribute;
    
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            current_screen[y][x] = blank;
        }
    }

    for(int i = 0; i < BUFFER_LINES; i++){
        line_len[i] = 0;
    }
    
    cursor_x = 0;
    cursor_y = 0;
    scroll_offset = 0;
}

// Add a line to scrollback buffer
static void add_line_to_buffer(uint16_t *line)
{
    if (follow_bottom) return;
    
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
    if (follow_bottom || scroll_offset == 0) {
        // Show current screen
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                vga_buffer[y * SCREEN_WIDTH + x] = current_screen[y][x];
            }
        }
        return;
    }

    // Show scrolled view
    // Calculate which lines to display (It works, don't bother double checking)
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
        
    // Show scroll indicator in top-right corner because it looks cool
    vga_buffer[79] = '^' | 0x0E00;  // Yellow up arrow
}

// Scroll screen up by one line
// Used only for automatic scrolling after \n or writing more than SCREEN_WIDTH chars
static inline void scroll_screen(void)
{
    uint16_t blank = ' ' | current_attribute;
    
    // Save top line to scrollback buffer
    if (!follow_bottom) {
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
        
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') { // 4 space tab
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            screen_invert_color();
            cursor_x--;
            current_screen[cursor_y][cursor_x] = ' ' | current_attribute;
            if (scroll_offset == 0) {
                vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = ' ' | current_attribute;
            }
        }
    } else {
        int source_line = buffer_count - scroll_offset + cursor_y;
        int buffer_idx = (buffer_start + source_line) % BUFFER_LINES;

        // Update Line Length.
        line_len[buffer_idx] = cursor_x+1;
        
        current_screen[cursor_y][cursor_x] = c | current_attribute;
        if (scroll_offset == 0) {
            vga_buffer[cursor_y * SCREEN_WIDTH + cursor_x] = c | current_attribute;
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
    uint16_t old_attr = current_attribute;
    current_attribute = vga_attribute_byte(fg, bg);
    screen_write(str);
    current_attribute = old_attr;
}

// Set color
void screen_set_color(vga_color fg, vga_color bg)
{
    current_attribute = vga_attribute_byte(fg, bg);
}

// 
uint8_t screen_get_line_len(void){
    int source_line = buffer_count - scroll_offset + cursor_y;
    int buffer_idx = (buffer_start + source_line) % BUFFER_LINES;

    return line_len[buffer_idx];
}

// Inverts the color where the cursor is.
void screen_invert_color(void){
    uint16_t *cursor = vga_buffer + cursor_y * SCREEN_WIDTH + cursor_x;
    uint8_t attr = *cursor >> 8;
    *cursor = (*cursor & 0xFF) | (((attr << 4) | (attr >> 4)) << 8); // cool one liner
    // current char~~^          mirrored attributes ~~~^
}

// 
void screen_clear_last_word(void){
    uint16_t blank = ' ' | current_attribute;

    int source_line = buffer_count - scroll_offset + cursor_y;
    int buffer_idx = (buffer_start + source_line) % BUFFER_LINES;
    
    line_len[buffer_idx] = 0;
    for(int i = 0; i < SCREEN_WIDTH; i++){
        line_buffer[buffer_idx][i] = blank;
        current_screen[cursor_y][i] = blank;
    }

    refresh_display();
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
    if (follow_bottom) return;
    
    // Maximum scroll is total lines minus screen height
    int max_scroll = buffer_count;
    
    if (scroll_offset < max_scroll) {
        scroll_offset += 1;  // Scroll by 1 line at a time
        if (scroll_offset > max_scroll) {
            scroll_offset = max_scroll;
        }
        refresh_display();
    }
}

// Scroll down (show newer content)
void screen_scroll_down(void)
{
    if (follow_bottom) return;
    
    if (scroll_offset > 0) {
        scroll_offset -= 1;  // Scroll by 1 line at a time
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