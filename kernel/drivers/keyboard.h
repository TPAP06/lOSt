// kernel/drivers/keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#include "kbd_LUTs.h"

typedef enum{
    PRESS,
    RELEASE
}key_action;

typedef enum{
    KBD_CTRL        = 1 >> 0,
    KBD_CAPS_LOCK   = 1 >> 1,
    KBD_ALT         = 1 >> 2,
    KBD_SHIFT_R     = 1 >> 3,
    KBD_SHIFT_L     = 1 >> 4
}kbd_mods;

typedef enum{
    KEY_FLAG_EXTENDED
}key_flags;

typedef struct{
    keycode_t code;     // A 1:1 map of every key on a keyboard using an enum
    key_action action;  // Press/Release (Add repeat later)
    kbd_mods mods;     // Modifications (ctrl/shift/etc)
    key_flags flags;    // Multi byte sequencies (E0)
    // Add time later
}key_event;

// initialize the keyboard handler
void kbd_handler_init(void);

// Reads a scancode and returns a key_event
bool kbd_pop_event(key_event *out);



#endif // KEYBOARD_H