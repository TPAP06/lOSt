// kernel/input/-----.c

#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "input_event.h"

// 
static kbd_mods ks = {0};

static bool CapsLock_down = 0;

static bool update_kbd_state(key_event ev){
    if(ev.action == PRESS){   // Handle key press
        if((ev.mods & KBD_CAPS_LOCK) && !CapsLock_down){ // If this is the first time pressing CapsLock
            CapsLock_down = true;   // "Remember" Caps Lock pressed
            ks ^= KBD_CAPS_LOCK;    // Toggle Caps Lock
            return true;
        }
        else if(ev.mods){    // If a mod other than capsLock was pressed
            ks |= ev.mods;   // Add it to the ks
            return true;
        }
    }
    else if(ev.action == RELEASE){    // Handle key release
        if(ev.mods & KBD_CAPS_LOCK) {
            // If Caps Lock released "remember it"
            CapsLock_down = false;
            return true;
        }
        else if(ev.mods){
            // Otherwise remove the released mod(s) from the kbd state
            ks &= ~ev.mods;
            return true;
        }
    }
    return false;
}

void kbd_get_state(kbd_mods *kbd_state){
    *kbd_state = ks;
}

// Non blocking (decide if you want to change that)
bool kbd_get_key(input_event *key){

    key_event event;

    // error handling 
    if(!kbd_pop_event(&event)) return false;

    // Update keyboard state
    update_kbd_state(event);

    key->type = INPUT_EV_KEY;
    key->mods = ks;
    key->key.code = event.code;
    key->key.action = event.action;
    key->key.flags = event.flags;

    return true;
}

static inline bool isLetter(keycode_t kc){
    return (kc >= KEY_A && kc <= KEY_Z);
}

static inline uint32_t KeycodeToText(keycode_t kc){
    // This excessive use of variables can be avoided by type casting to bool
    // But it would make the code unreadable
    bool shift = (ks & KBD_SHIFT_L) | (ks & KBD_SHIFT_R);
    bool caps = ks & KBD_CAPS_LOCK;
    bool upper = shift ^ caps;
    if(isLetter(kc)) return (upper) ? keycode_to_text_us_shift[kc] : keycode_to_text_us[kc];
    return shift ? keycode_to_text_us_shift[kc] : keycode_to_text_us[kc];
}

// Get a key press in form of text
bool kbd_get_text(input_event *text){
    key_event event;

    // Return false if queue empty or key code invalid
    if(!kbd_pop_event(&event))  return false;
    if(event.code > KEY_MAX)    return false;

    // Update keyboard state
    if(update_kbd_state(event)) return false;   // If the key pressed was a mod (mod != char) throw it
    
    if(event.action == RELEASE) return false;
    
    text->type = INPUT_EV_TEXT;
    text->mods = ks;
    text->text.codepoint = KeycodeToText(event.code);
    return true;
}