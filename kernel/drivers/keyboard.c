// kernel/drivers/keyboard.c

#include "keyboard.h"
#include "../tty/screen_edit.h"
#include "../interrupts/isr.h"
#include "../lib/io.h"
#include "../lib/string.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keeps track of the keyboard state
static kbd_state ks = 0;

void kbd_pop_event(key_event *out){
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // Init to empty event
    *out = (key_event){0};

    // Set "pressed"
    out->action = (scancode & 0x80) ? RELEASE : PRESS;

    // Check for extended
    if(scancode == 0xE0){
        ks |= KBD_EXTENDED;
        out->flags = KEY_FLAG_EXTENDED;
        return;
    }

    // Set mods
    switch (scancode)
    {
    case 0x1D: out->mods = KBD_CTRL;        ks |= KBD_CTRL;      return; // Same code for both right & left Ctrl
    case 0x2A: out->mods = KBD_SHIFT_L;     ks |= KBD_SHIFT_L;   return; // Left Shift
    case 0x36: out->mods = KBD_SHIFT_R;     ks |= KBD_SHIFT_R;   return; // Right Shift
    case 0x38: out->mods = KBD_ALT;         ks |= KBD_ALT;       return; // Same code for both right & left (ALTGR) ALT
    case 0x3A: out->mods = KBD_CAPS_LOCK;   ks |= KBD_CAPS_LOCK; return; // CapsLock
    }
    
    // Set keycode
    out->code = ps2_set1_keycode[scancode&0x7F];
    return;
}