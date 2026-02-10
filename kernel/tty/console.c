// kernel/console/console.c

#include "../drivers/screen.h"
#include "../input/input_kbd.h"
#include "../lib/string.h"

void console_run(void){
    input_event ev;
    while(1){
        while(!kbd_get_text(&ev)){
            __asm__ volatile ("hlt");
        }
        if(ev.text.codepoint == 'a') vga_putchar(5,5, '^' , 0x0E00);
    }
}