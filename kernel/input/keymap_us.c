// kernel/drivers/keyboard.c

#include "../drivers/keyboard.h"
#include "../lib/string.h"

// 
static kbd_state ks = 0;

void funct(){

    key_event event;
    kbd_pop_event(&event);

    // Update kbd state
    if(event.flags & KEY_FLAG_EXTENDED){
        ks |= KBD_EXTENDED;
    }

    // Handle extended sc
    if(ks & KBD_EXTENDED){
        ks ^= KBD_EXTENDED;
        // add extended lut
        return;
    }

        char c;
    if(scancode < sizeof(scancode_to_ascii)){
        if(ks & SHIFT_L || ks & SHIFT_R)   c = scancode_to_ascii_shift[scancode];
        else                c = scancode_to_ascii[scancode];
        return;
    }

    // Handle non mod, extended, char scancodes


}