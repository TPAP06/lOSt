// kernel/console/screen_edit.c
// Use after heap

#include "screen_edit.h"
#include "../drivers/screen.h"
#include "../input/input_kbd.h"
#include "../lib/string.h"
#include "../memory/heap.h"

// Cursor position
static int cursor_x = 0;
static int cursor_y = 0;

// Current color
static uint16_t current_attribute;

// Scrollback buffer (cyclical buffer of lines)
static sb_t sb;

// Holds the possition of the last char in each
static uint8_t line_len[SCROLLBACK_LINES];

// Create VGA attribute byte
uint16_t get_attr_byte(vga_color fg, vga_color bg)
{
    return (fg | (bg << 4)) << 8;
}

static inline uint16_t get_idx(uint16_t x, uint16_t y){ return y * SCREEN_WIDTH + x; }

// Initialize screen
void screen_init(void)
{
    cursor_x = 0;
    cursor_y = 0;
    current_attribute = 0x0700; // Light-Grey fg and Black bg. Otherwise use get_attr_byte

    // can't malloc on init because it occurs before heap init
    sb.buffer    = (uint16_t*)calloc(SCROLLBACK_LINES  * SCREEN_WIDTH, sizeof(uint16_t));

    // This function currently has no use
    // vga_init();

    screen_clear();
}

// Clear screen
void screen_clear(void)
{
    vga_clear();
    
    uint16_t blank = ' ' | current_attribute;
    
    for(int i = 0; i < SCROLLBACK_LINES; i++){
        line_len[i] = 0;
    }
    for(size_t i = 0; i < SCROLLBACK_LINES * SCREEN_WIDTH; i++){
        sb.buffer[i] = blank;
    }

    // Init cursor top left
    cursor_x = 0;
    cursor_y = 0;

    // Clear ScrollBack variables
    sb.view_top = sb.count = sb.scroll_offset = sb.start = 0;
    sb.follow_bottom = true;
}

// Add a line to scrollback buffer
// static void add_line_to_buffer(uint16_t *line)
// {
//     if (sb.follow_bottom) return;
    
//     int write_pos = (sb.start + sb.count) % SCROLLBACK_LINES;
    
//     // Copy line to buffer
//     for (int x = 0; x < SCREEN_WIDTH; x++) {
//         sb.buffer[get_idx(x,write_pos)] = line[x];
//     }
    
//     if (sb.count < SCROLLBACK_LINES) {
//         sb.count++;
//     } else {
//         // Buffer full, overwrite oldest
//         sb.start = (sb.start + 1) % SCROLLBACK_LINES;
//     }
// }

// Refresh VGA display based on scroll offset
static void refresh_display(void)
{
    uint16_t *fullscreen = &sb.buffer[sb.view_top];
    vga_refresh(fullscreen, SCREEN_HEIGHT * SCREEN_WIDTH);

    // Show scroll indicator in top-right corner because it looks cool
    vga_putchar(SCREEN_WIDTH-1, 0, '^' | 0x0E00);    // Yellow up arrow
}

// Scroll screen up by one line
// Used only for automatic scrolling after \n or writing more than SCREEN_WIDTH chars
static inline void scroll_screen(void)
{
    // uint16_t blank = ' ' | current_attribute;
    
    // Save top line to scrollback buffer
    if (!sb.follow_bottom) {
        // add_line_to_buffer(current_screen[0]);
    }
    
    cursor_y = SCREEN_HEIGHT - 1;
    
    // Refresh VGA if we're at bottom
    if (sb.scroll_offset == 0) {
        refresh_display();
    }
}

// Put character on screen
void screen_putchar(char c)
{
    // If scrolled up, jump to bottom on new input
    if (sb.scroll_offset > 0) {
        sb.scroll_offset = 0;
        refresh_display();
    }
        
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') { // 4 space tab
        // must add blank for this jump
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            // update scroll sb.buffer
            // insert mode == 1: move line to the left by one
            // current_screen[cursor_y * SCREEN_WIDTH + cursor_x] = ' ' | current_attribute;
            cursor_x--;
            vga_putchar(cursor_x, cursor_y, ' ' | current_attribute);
        }
    } else {
        int source_line = sb.count - sb.scroll_offset + cursor_y;
        int buffer_idx = (sb.start + source_line) % SCROLLBACK_LINES;

        // Update Line Length.
        line_len[buffer_idx] = cursor_x+1;
        
        // update scroll sb.buffer
        // current_screen[cursor_y * SCREEN_WIDTH + cursor_x] = c | current_attribute;
        vga_putchar(cursor_x, cursor_y, c | current_attribute);
        cursor_x++;
    }

    // Modulize cursor
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= SCREEN_HEIGHT) {
        scroll_screen();
    }

    vga_set_cursor(cursor_x, cursor_y);
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
    current_attribute = get_attr_byte(fg, bg);
    screen_write(str);
    current_attribute = old_attr;
}

// Set color
void screen_set_color(vga_color fg, vga_color bg){
    current_attribute = get_attr_byte(fg, bg);
}

// 
uint8_t screen_get_line_len(void){
    int source_line = sb.count - sb.scroll_offset + cursor_y;
    int buffer_idx = (sb.start + source_line) % SCROLLBACK_LINES;

    return line_len[buffer_idx];
}

// Clear all chars until whitespace
void screen_clear_last_word(void){
    
    // later make it detected mode:
    // mode 0: starting on != blank char and clear up until you hit == blank
    // mode 1: starting on == blank char and clear up until you hit != blank
    uint16_t blank = ' ' | current_attribute;

    // left margin
    for(; cursor_x <= 2; cursor_x--){
        // Stop at first whitespace
        if(sb.buffer[get_idx(cursor_x, cursor_y)] == blank) break; 

        sb.buffer[get_idx(cursor_x, cursor_y)] = blank;
    }

    // Modulize cursor_x
    // if(cursor_x <= LEFT_MARGIN) cursor_x = LEFT_MARGIN

    // Update line_len
    line_len[sb.view_top + cursor_y] = cursor_x;

    // sanitize
    vga_set_row(&sb.buffer[sb.view_top + cursor_y], cursor_y);
}

// Set cursor
void screen_set_cursor(uint16_t x, uint16_t y){
    vga_set_cursor(x, y);
}

// Scroll up (show older content)
void screen_scroll_up(void)
{
    if(sb.follow_bottom) return;
    // Don't allow to scroll for more lines that have already been writen in the buffer
    if(sb.scroll_offset >= sb.count) return;
 
    sb.scroll_offset++;
    // sanity/ valid check
    sb.view_top = (sb.view_top == 0) ? SCROLLBACK_LINES - 1 : sb.view_top - 1;

    // Modulize sb.scroll_offset (can't be larger than sb.count)
    if(sb.scroll_offset > sb.count) sb.scroll_offset = sb.count;
    
    refresh_display();
}

// Scroll down (show newer content)
void screen_scroll_down(void)
{
    if(sb.follow_bottom) return;
    // You are already at bottom, don't allow to go lower (cause wrap-around)
    if(sb.scroll_offset <= 0) return;

    sb.scroll_offset--;
    refresh_display();
}

// Jump to bottom and refresh
void screen_scroll_to_bottom(void)
{
    if (sb.scroll_offset == 0) return;
    
    sb.view_top += sb.scroll_offset;
    sb.scroll_offset = 0;

    refresh_display();
}