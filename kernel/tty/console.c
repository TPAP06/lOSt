// kernel/console/console.c

#include "../drivers/screen.h"
#include "../input/input_kbd.h"
#include "../lib/string.h"

#include "screen_edit.h"

#define LEFT_MARGIN 2

// Cursor position
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Current color
static uint16_t current_attribute;
void console_run(void){
    input_event ev;
    while(1){
        while(!kbd_get_text(&ev)){
            __asm__ volatile ("hlt");
        }

        uint32_t c = ev.text.codepoint;
        screen_putchar((char)c);
    }
}

void console_run2(void){
    input_event ev;
    
    vga_set_cursor(cursor_x, cursor_y);
    
    while(1){
        while(!kbd_get_text(&ev)){
            __asm__ volatile ("hlt");
        }

        uint32_t c = ev.text.codepoint;
        kbd_mods mods = ev.mods;

        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
        }
        else if (c == '\b') {
            if(mods & KBD_CTRL){
                // screen_clear_last_word();
            }
            else if(cursor_x > LEFT_MARGIN){
                cursor_x--;
                vga_putchar(cursor_x, cursor_y, ' ' | 0x0700);
            }
        }
        // else if (c == KEY_UP_ARROW) {
        //     // Use ctrl_pressed directly (it's in this file)
        //     if (ctrl_pressed) {
        //         // Ctrl+Up = Scroll up
        //         screen_scroll_up();
        //     } else {
        //         // Regular Up = Command history
        //         if (history_count == 0) {
        //             continue;
        //         }
                
        //         if (history_offset == 0) {
        //             strcpy(temp_input, buffer);
        //         }
                
        //         if (history_offset > -(history_count)) {
        //             history_offset--;
                    
        //             int hist_idx = (*history_write_pos + history_offset + history_max) % history_max;
                    
        //             screen_set_cursor(start_x, start_y);
        //             for (int i = 0; i < pos; i++) {
        //                 screen_putchar(' ');
        //             }
        //             screen_set_cursor(start_x, start_y);
                    
        //             strcpy(buffer, history[hist_idx]);
        //             pos = strlen(buffer);
        //             screen_write(buffer);
        //         }
        //     }
        // }
        // else if (c == KEY_DOWN_ARROW) {
        //     // Use ctrl_pressed directly
        //     if (ctrl_pressed) {
        //         // Ctrl+Down = Scroll down
        //         screen_scroll_down();
        //     } else {
        //         // Regular Down = Command history
        //         if (history_offset < 0) {
        //             history_offset++;
                    
        //             screen_set_cursor(start_x, start_y);
        //             for (int i = 0; i < pos; i++) {
        //                 screen_putchar(' ');
        //             }
        //             screen_set_cursor(start_x, start_y);
                    
        //             if (history_offset == 0) {
        //                 strcpy(buffer, temp_input);
        //             } else {
        //                 int hist_idx = (*history_write_pos + history_offset + history_max) % history_max;
        //                 strcpy(buffer, history[hist_idx]);
        //             }
                    
        //             pos = strlen(buffer);
        //             screen_write(buffer);
        //         }
        //     }
        // }
        // else if(c == KEY_LEFT_ARROW){
        //     //  if use ctrl just to start of line (0 is reserved so the line starts at x = 1).

        //     if(ctrl_pressed){
        //         pos = 0;
        //         screen_set_cursor(2, start_y);
        //     } else if(pos > 0){
        //         pos--;
        //         // Regular Left = move by one to the left
        //         screen_set_cursor(start_x-1, start_y);
        //     }
        // }
        // else if(c == KEY_RIGHT_ARROW){
            
        //     uint8_t line_len = screen_get_line_len();
            
        //     if(ctrl_pressed){
        //         pos = line_len;
        //         // Jump to the end of line
        //         screen_set_cursor(line_len, start_y);
        //     } else if(start_x < line_len){  // Hardcoded to not allow the cursor to move on the void
        //         pos++;
        //         // Regular Left = move by one to the left
        //         screen_set_cursor(start_x+1, start_y);
        //     }
        // }
        else if (c >= 32 && c < 127) {
            vga_putchar(cursor_x, cursor_y, c | 0x0E00);
            cursor_x++;
        }

        // Modulize cursor
        if(cursor_x >= SCREEN_WIDTH){
            cursor_x = 0;
            cursor_y++;
        }
        vga_set_cursor(cursor_x, cursor_y);
    }
}