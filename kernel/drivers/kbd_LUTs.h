// kernel/drivers/keyboard.h

#ifndef KBD_LUTS_H
#define KBD_LUTS_H

#include <stdint.h>
#include <stdbool.h>

// A keycode is a 1:1 match of a scancode to a full (100%) US keyboard
typedef enum{
    KEY_NONE = 0x0000,

    // Letters
    KEY_A = 0x0004, KEY_B = 0x0005, KEY_C = 0x0006, KEY_D = 0x0007,
    KEY_E = 0x0008, KEY_F = 0x0009, KEY_G = 0x000A, KEY_H = 0x000B,
    KEY_I = 0x000C, KEY_J = 0x000D, KEY_K = 0x000E, KEY_L = 0x000F,
    KEY_M = 0x0010, KEY_N = 0x0011, KEY_O = 0x0012, KEY_P = 0x0013,
    KEY_Q = 0x0014, KEY_R = 0x0015, KEY_S = 0x0016, KEY_T = 0x0017,
    KEY_U = 0x0018, KEY_V = 0x0019, KEY_W = 0x001A, KEY_X = 0x001B,
    KEY_Y = 0x001C, KEY_Z = 0x001D,

    // Numbers top row
    KEY_1 = 0x001E, KEY_2 = 0x001F, KEY_3 = 0x0020, KEY_4 = 0x0021,
    KEY_5 = 0x0022, KEY_6 = 0x0023, KEY_7 = 0x0024, KEY_8 = 0x0025,
    KEY_9 = 0x0026, KEY_0 = 0x0027,

    // Control / punctuation
    KEY_ENTER      = 0x0028,
    KEY_ESC        = 0x0029,
    KEY_BACKSPACE  = 0x002A,
    KEY_TAB        = 0x002B,
    KEY_SPACE      = 0x002C,
    KEY_MINUS      = 0x002D, // -
    KEY_EQUAL      = 0x002E, // =
    KEY_LBRACKET   = 0x002F, // [
    KEY_RBRACKET   = 0x0030, // ]
    KEY_BACKSLASH  = 0x0031, // \  (US)
    KEY_SEMICOLON  = 0x0033, // ;
    KEY_APOSTROPHE = 0x0034, // '
    KEY_GRAVE      = 0x0035, // `
    KEY_COMMA      = 0x0036, // ,
    KEY_DOT        = 0x0037, // .
    KEY_SLASH      = 0x0038, // /
    KEY_CAPSLOCK   = 0x0039,

    // Function keys
    KEY_F1  = 0x003A, KEY_F2  = 0x003B, KEY_F3  = 0x003C, KEY_F4  = 0x003D,
    KEY_F5  = 0x003E, KEY_F6  = 0x003F, KEY_F7  = 0x0040, KEY_F8  = 0x0041,
    KEY_F9  = 0x0042, KEY_F10 = 0x0043, KEY_F11 = 0x0044, KEY_F12 = 0x0045,

    // System / navigation
    KEY_PRINTSCREEN = 0x0046,
    KEY_SCROLLLOCK  = 0x0047,
    KEY_PAUSE       = 0x0048,
    KEY_INSERT      = 0x0049,
    KEY_HOME        = 0x004A,
    KEY_PAGEUP      = 0x004B,
    KEY_DELETE      = 0x004C,
    KEY_END         = 0x004D,
    KEY_PAGEDOWN    = 0x004E,
    KEY_RIGHT       = 0x004F,
    KEY_LEFT        = 0x0050,
    KEY_DOWN        = 0x0051,
    KEY_UP          = 0x0052,

    // Keypad
    KEY_NUMLOCK     = 0x0053,
    KEY_KP_SLASH    = 0x0054,
    KEY_KP_ASTERISK = 0x0055,
    KEY_KP_MINUS    = 0x0056,
    KEY_KP_PLUS     = 0x0057,
    KEY_KP_ENTER    = 0x0058,
    KEY_KP_1        = 0x0059,
    KEY_KP_2        = 0x005A,
    KEY_KP_3        = 0x005B,
    KEY_KP_4        = 0x005C,
    KEY_KP_5        = 0x005D,
    KEY_KP_6        = 0x005E,
    KEY_KP_7        = 0x005F,
    KEY_KP_8        = 0x0060,
    KEY_KP_9        = 0x0061,
    KEY_KP_0        = 0x0062,
    KEY_KP_DOT      = 0x0063,

    KEY_NONUS_BSLASH = 0x0064, // 102nd key on ISO keyboards (<> / \|)

    KEY_APPLICATION  = 0x0065, // Menu key

    // Modifiers (HID places them at 0xE0..0xE7)
    KEY_LCTRL  = 0x00E0,
    KEY_LSHIFT = 0x00E1,
    KEY_LALT   = 0x00E2,
    KEY_LGUI   = 0x00E3, // Windows / Command
    KEY_RCTRL  = 0x00E4,
    KEY_RSHIFT = 0x00E5,
    KEY_RALT   = 0x00E6, // AltGr on many layouts
    KEY_RGUI   = 0x00E7,
}keycode_t;

#define KEY_MAX 0x00E7

#define MAP(sc, kc) [sc] = (kc)

static const keycode_t ps2_set1_to_keycode[128] = {
    // Don't need to manually init it to zero since the compiler does that due to the var being static
    // [0 ... 127] = KEY_NONE,

    MAP(0x01, KEY_ESC),
    MAP(0x02, KEY_1), MAP(0x03, KEY_2), MAP(0x04, KEY_3), MAP(0x05, KEY_4),
    MAP(0x06, KEY_5), MAP(0x07, KEY_6), MAP(0x08, KEY_7), MAP(0x09, KEY_8),
    MAP(0x0A, KEY_9), MAP(0x0B, KEY_0), MAP(0x0C, KEY_MINUS), MAP(0x0D, KEY_EQUAL),
    MAP(0x0E, KEY_BACKSPACE),
    MAP(0x0F, KEY_TAB),

    MAP(0x10, KEY_Q), MAP(0x11, KEY_W), MAP(0x12, KEY_E), MAP(0x13, KEY_R),
    MAP(0x14, KEY_T), MAP(0x15, KEY_Y), MAP(0x16, KEY_U), MAP(0x17, KEY_I),
    MAP(0x18, KEY_O), MAP(0x19, KEY_P), MAP(0x1A, KEY_LBRACKET), MAP(0x1B, KEY_RBRACKET),
    MAP(0x1C, KEY_ENTER),
    MAP(0x1D, KEY_LCTRL),

    MAP(0x1E, KEY_A), MAP(0x1F, KEY_S), MAP(0x20, KEY_D), MAP(0x21, KEY_F),
    MAP(0x22, KEY_G), MAP(0x23, KEY_H), MAP(0x24, KEY_J), MAP(0x25, KEY_K),
    MAP(0x26, KEY_L), MAP(0x27, KEY_SEMICOLON), MAP(0x28, KEY_APOSTROPHE),
    MAP(0x29, KEY_GRAVE),

    MAP(0x2A, KEY_LSHIFT),
    MAP(0x2B, KEY_BACKSLASH),
    MAP(0x2C, KEY_Z), MAP(0x2D, KEY_X), MAP(0x2E, KEY_C), MAP(0x2F, KEY_V),
    MAP(0x30, KEY_B), MAP(0x31, KEY_N), MAP(0x32, KEY_M),
    MAP(0x33, KEY_COMMA), MAP(0x34, KEY_DOT), MAP(0x35, KEY_SLASH),
    MAP(0x36, KEY_RSHIFT),

    MAP(0x37, KEY_KP_ASTERISK),  // keypad *
    MAP(0x38, KEY_LALT),
    MAP(0x39, KEY_SPACE),
    MAP(0x3A, KEY_CAPSLOCK),

    MAP(0x3B, KEY_F1), MAP(0x3C, KEY_F2), MAP(0x3D, KEY_F3), MAP(0x3E, KEY_F4),
    MAP(0x3F, KEY_F5), MAP(0x40, KEY_F6), MAP(0x41, KEY_F7), MAP(0x42, KEY_F8),
    MAP(0x43, KEY_F9), MAP(0x44, KEY_F10),

    MAP(0x45, KEY_NUMLOCK),
    MAP(0x46, KEY_SCROLLLOCK),

    MAP(0x47, KEY_KP_7),
    MAP(0x48, KEY_KP_8),
    MAP(0x49, KEY_KP_9),
    MAP(0x4A, KEY_KP_MINUS),
    MAP(0x4B, KEY_KP_4),
    MAP(0x4C, KEY_KP_5),
    MAP(0x4D, KEY_KP_6),
    MAP(0x4E, KEY_KP_PLUS),
    MAP(0x4F, KEY_KP_1),
    MAP(0x50, KEY_KP_2),
    MAP(0x51, KEY_KP_3),
    MAP(0x52, KEY_KP_0),
    MAP(0x53, KEY_KP_DOT),

    MAP(0x56, KEY_NONUS_BSLASH), // ISO 102nd key on many keyboards
    MAP(0x57, KEY_F11),
    MAP(0x58, KEY_F12),
};

static const keycode_t ps2_set1_e0_to_keycode[128] = {
    // Don't need to manually init it to zero since the compiler does that due to the var being static
    // [0 ... 127] = KEY_NONE,

    MAP(0x1C, KEY_KP_ENTER),
    MAP(0x1D, KEY_RCTRL),

    MAP(0x35, KEY_KP_SLASH),
    MAP(0x38, KEY_RALT),

    MAP(0x47, KEY_HOME),
    MAP(0x48, KEY_UP),
    MAP(0x49, KEY_PAGEUP),
    MAP(0x4B, KEY_LEFT),
    MAP(0x4D, KEY_RIGHT),
    MAP(0x4F, KEY_END),
    MAP(0x50, KEY_DOWN),
    MAP(0x51, KEY_PAGEDOWN),
    MAP(0x52, KEY_INSERT),
    MAP(0x53, KEY_DELETE),

    // 104-key Windows keys + Menu:
    MAP(0x5B, KEY_LGUI),
    MAP(0x5C, KEY_RGUI),
    MAP(0x5D, KEY_APPLICATION),
};

static const uint8_t keycode_to_text_us[KEY_MAX + 1] = {
    // Don't need to manually init it to zero since the compiler does that due to the var being static
    // [0 ... KEY_MAX] = 0,

    [KEY_A] = 'a', [KEY_B] = 'b', [KEY_C] = 'c', [KEY_D] = 'd',
    [KEY_E] = 'e', [KEY_F] = 'f', [KEY_G] = 'g', [KEY_H] = 'h',
    [KEY_I] = 'i', [KEY_J] = 'j', [KEY_K] = 'k', [KEY_L] = 'l',
    [KEY_M] = 'm', [KEY_N] = 'n', [KEY_O] = 'o', [KEY_P] = 'p',
    [KEY_Q] = 'q', [KEY_R] = 'r', [KEY_S] = 's', [KEY_T] = 't',
    [KEY_U] = 'u', [KEY_V] = 'v', [KEY_W] = 'w', [KEY_X] = 'x',
    [KEY_Y] = 'y', [KEY_Z] = 'z',

    [KEY_1] = '1', [KEY_2] = '2', [KEY_3] = '3', [KEY_4] = '4',
    [KEY_5] = '5', [KEY_6] = '6', [KEY_7] = '7', [KEY_8] = '8',
    [KEY_9] = '9', [KEY_0] = '0',

    [KEY_SPACE] = ' ',
    [KEY_TAB] = '\t',
    [KEY_ENTER] = '\n',

    [KEY_MINUS] = '-',
    [KEY_EQUAL] = '=',
    [KEY_LBRACKET] = '[',
    [KEY_RBRACKET] = ']',
    [KEY_BACKSLASH] = '\\',
    [KEY_SEMICOLON] = ';',
    [KEY_APOSTROPHE] = '\'',
    [KEY_GRAVE] = '`',
    [KEY_COMMA] = ',',
    [KEY_DOT] = '.',
    [KEY_SLASH] = '/',

    // keypad "text" (NumLock-on convention)
    [KEY_KP_ENTER] = '\n',
    [KEY_KP_SLASH] = '/',
    [KEY_KP_ASTERISK] = '*',
    [KEY_KP_MINUS] = '-',
    [KEY_KP_PLUS] = '+',
    [KEY_KP_DOT] = '.',
    [KEY_KP_0] = '0', [KEY_KP_1] = '1', [KEY_KP_2] = '2', [KEY_KP_3] = '3',
    [KEY_KP_4] = '4', [KEY_KP_5] = '5', [KEY_KP_6] = '6', [KEY_KP_7] = '7',
    [KEY_KP_8] = '8', [KEY_KP_9] = '9',
};

static const uint8_t keycode_to_text_us_shift[KEY_MAX + 1] = {
    // Don't need to manually init it to zero since the compiler does that due to the var being static
    // [0 ... KEY_MAX] = 0,

    // Letters: you usually apply caps/shift logic in code, but if you want a pure shift table:
    [KEY_A] = 'A', [KEY_B] = 'B', [KEY_C] = 'C', [KEY_D] = 'D',
    [KEY_E] = 'E', [KEY_F] = 'F', [KEY_G] = 'G', [KEY_H] = 'H',
    [KEY_I] = 'I', [KEY_J] = 'J', [KEY_K] = 'K', [KEY_L] = 'L',
    [KEY_M] = 'M', [KEY_N] = 'N', [KEY_O] = 'O', [KEY_P] = 'P',
    [KEY_Q] = 'Q', [KEY_R] = 'R', [KEY_S] = 'S', [KEY_T] = 'T',
    [KEY_U] = 'U', [KEY_V] = 'V', [KEY_W] = 'W', [KEY_X] = 'X',
    [KEY_Y] = 'Y', [KEY_Z] = 'Z',

    // Number row shifted symbols (US)
    [KEY_1] = '!', [KEY_2] = '@', [KEY_3] = '#', [KEY_4] = '$',
    [KEY_5] = '%', [KEY_6] = '^', [KEY_7] = '&', [KEY_8] = '*',
    [KEY_9] = '(', [KEY_0] = ')',

    [KEY_SPACE] = ' ',
    [KEY_TAB] = '\t',
    [KEY_ENTER] = '\n',

    [KEY_MINUS] = '_',
    [KEY_EQUAL] = '+',
    [KEY_LBRACKET] = '{',
    [KEY_RBRACKET] = '}',
    [KEY_BACKSLASH] = '|',
    [KEY_SEMICOLON] = ':',
    [KEY_APOSTROPHE] = '"',
    [KEY_GRAVE] = '~',
    [KEY_COMMA] = '<',
    [KEY_DOT] = '>',
    [KEY_SLASH] = '?',

    // keypad usually ignores shift for text; keep same as normal if you want:
    [KEY_KP_ENTER] = '\n',
    [KEY_KP_SLASH] = '/',
    [KEY_KP_ASTERISK] = '*',
    [KEY_KP_MINUS] = '-',
    [KEY_KP_PLUS] = '+',
    [KEY_KP_DOT] = '.',
    [KEY_KP_0] = '0', [KEY_KP_1] = '1', [KEY_KP_2] = '2', [KEY_KP_3] = '3',
    [KEY_KP_4] = '4', [KEY_KP_5] = '5', [KEY_KP_6] = '6', [KEY_KP_7] = '7',
    [KEY_KP_8] = '8', [KEY_KP_9] = '9',
};

#undef MAP  // Don't really need it no more

#endif // KBD_LUTS_H