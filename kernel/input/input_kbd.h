// kernel/input/input_kbd.h

#ifndef INPUT_KBD_H
#define INPUT_KBD_H

#include <stdint.h>
#include <stdbool.h>

#include "../drivers/keyboard.h"
#include "input_event.h"

// Get the current state of the keyboard (Shift, Ctrl, CapsLock, etc)
void kbd_get_state(kbd_mods *kbd_state);

// Get a key even from the keyboard queue
bool kbd_get_key(input_event *key);

// Get a text even from the keyboard queue
bool kbd_get_text(input_event *text);

#endif // INPUT_KBD_H