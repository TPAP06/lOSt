// kernel/tty/screen_edit.h

#ifndef SCREEN_EDIT_H
#define SCREEN_EDIT_H

#include <stdint.h>
#include "../drivers/screen.h"

void screen_init(void);
void screen_clear(void);
void screen_putchar(char c);
void screen_write(const char* str);
void screen_write_color(const char* str, vga_color fg, vga_color bg);

void screen_set_color(vga_color fg, vga_color bg);
void screen_clear_last_word(void);
void screen_set_cursor(uint16_t x, uint16_t y);
void screen_scroll_up(void);
void screen_scroll_down(void);
void screen_scroll_to_bottom(void);


uint8_t screen_get_line_len(void);

#endif // SCREEN_EDIT_H