// kernel/drivers/keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// A keycode is a 1:1 match of a scancode to a keybutton
typedef enum{
    KEY_NONE,   KEY_ESC,    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,    KEY_F11,    KEY_F12,
    KEY_GRAVE,  KEY_1,  KEY_2,  KEY_3,  KEY_4,  KEY_5,  KEY_6,  KEY_7,  KEY_8,  KEY_9,  KEY_0,  KEY_MINUS,  KEY_EQUAL,  KEY_BACKSPACE,
    KEY_TAB,    KEY_Q,  KEY_W,  KEY_E,  KEY_R,  KEY_T,  KEY_Y,  KEY_U,  KEY_I,  KEY_O,  KEY_P,  KEY_LBRACKET,   KEY_RBRACKET,   KEY_ENTER,
    KEY_CAPSLOCK,   KEY_A,  KEY_S,  KEY_D,  KEY_F,  KEY_G,  KEY_H,  KEY_J,  KEY_K,  KEY_L,  KEY_SEMICOLON,  KEY_APOSTROPHE, KEY_BACKSLASH,
    KEY_LSHIFT, KEY_Z,  KEY_X,  KEY_C,  KEY_V,  KEY_B,  KEY_N,  KEY_M,  KEY_COMMA,  KEY_DOT,    KEY_SLASH,  KEY_RSHIFT,
    KEY_LCTRL,
    KEY_LALT,
    KEY_SPACE,
    KEY_NUMLOCK,
    KEY_SCROLLLOCK,
    KEY_KP_1,
    KEY_KP_2,
    KEY_KP_3,
    KEY_KP_4,
    KEY_KP_5,
    KEY_KP_6,
    KEY_KP_7,
    KEY_KP_8,
    KEY_KP_9,
    KEY_KP_0,
    KEY_KP_MINUS,
    KEY_KP_PLUS,
    KEY_KP_DOT
}keycode_t;

typedef enum{
    PRESS,
    RELEASE
}key_action;

typedef enum{
    KBD_CTRL        = 1 >> 0,
    KBD_CAPS_LOCK   = 1 >> 1,
    KBD_ALT         = 1 >> 2,
    KBD_SHIFT_R     = 1 >> 3,
    KBD_SHIFT_L     = 1 >> 4,
    KBD_EXTENDED    = 1 >> 5
}kbd_state;

typedef enum{
    KEY_FLAG_EXTENDED
}key_flags;

typedef struct{
    keycode_t code;      // 
    key_action action;  // 
    kbd_state mods;       // Modifications (ctrl/shift/etc)
    key_flags flags;    // Multi byte sequencies (E0)
}key_event;


// Set1 scancode to PS/2 US keycode
static const keycode_t ps2_set1_keycode[128] = {
    [0 ... 127] = KEY_NONE,
    [0X01] = KEY_ESC,
    [0X02] = KEY_1, 
    [0X03] = KEY_2,
    [0X04] = KEY_3,
    [0X05] = KEY_4,
    [0X06] = KEY_5,
    [0X07] = KEY_6,
    [0X08] = KEY_7,
    [0X09] = KEY_8,
    [0X0A] = KEY_9,
    [0X0B] = KEY_0,
    [0X0C] = KEY_MINUS,
    [0X0D] = KEY_EQUAL,
    [0X0E] = KEY_BACKSPACE,
    
    [0X0F] = KEY_TAB,
    [0X10] = KEY_Q,
    [0x11] = KEY_W,
    [0x12] = KEY_E,
    [0x13] = KEY_R,
    [0x14] = KEY_T,
    [0x15] = KEY_Y,
    [0x16] = KEY_U,
    [0x17] = KEY_I,
    [0x18] = KEY_O,
    [0x19] = KEY_P,
    [0x1A] = KEY_LBRACKET,
    [0x1B] = KEY_RBRACKET,
    [0x1C] = KEY_ENTER,
    [0x1D] = KEY_LCTRL,

    [0x1E] = KEY_A,
    [0x1F] = KEY_S,
    [0x20] = KEY_D,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,
    [0x27] = KEY_SEMICOLON,
    [0x28] = KEY_APOSTROPHE,
    [0x29] = KEY_GRAVE,      // `~

    [0x2A] = KEY_LSHIFT,
    [0x2B] = KEY_BACKSLASH,
    [0x2C] = KEY_Z,
    [0x2D] = KEY_X,
    [0x2E] = KEY_C,
    [0x2F] = KEY_V,
    [0x30] = KEY_B,
    [0x31] = KEY_N,
    [0x32] = KEY_M,
    [0x33] = KEY_COMMA,
    [0x34] = KEY_DOT,
    [0x35] = KEY_SLASH,
    [0x36] = KEY_RSHIFT,

    [0x38] = KEY_LALT,
    [0x39] = KEY_SPACE,
    [0x3A] = KEY_CAPSLOCK,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x45] = KEY_NUMLOCK,
    [0x46] = KEY_SCROLLLOCK,

    // Keypad
    [0x47] = KEY_KP_7,
    [0x48] = KEY_KP_8,
    [0x49] = KEY_KP_9,
    [0x4A] = KEY_KP_MINUS,
    [0x4B] = KEY_KP_4,
    [0x4C] = KEY_KP_5,
    [0x4D] = KEY_KP_6,
    [0x4E] = KEY_KP_PLUS,
    [0x4F] = KEY_KP_1,
    [0x50] = KEY_KP_2,
    [0x51] = KEY_KP_3,
    [0x52] = KEY_KP_0,
    [0x53] = KEY_KP_DOT,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12
};

// Reads a scancode and returns a key_event
void kbd_pop_event(key_event *out);

#endif // KEYBOARD_H