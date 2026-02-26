# Rules

- Drivers must only communicate with hardware
- Make sure to transfer raw data and apply no changes to it
- Keep the drivers minimal

# Keyboard Driver

Communicates with PS/2 console
Data port:  0x60
Status port:0x64

Useful structs/enums:

__key_event__ is a struct that contains:
- A 1:1 map of every key on a keyboard  -> __keycode_t code;__
- The action Press/Release              -> __key_action action;__
- Modifications such as Ctrl/Shift/Alt  -> __kbd_mods mods;__
- Multi byte sequencies aka Flags       -> __key_flags flags;__

- Example:  Read a key press and print if the key was 'a' and Shift or CapsLock was on
    ```c
    key_event ev;
    if(ev.action == RELEASE) return;    // Only register key press
    // ks is the current keyboard state
    bool shift = (ks & KBD_SHIFT_L) | (ks & KBD_SHIFT_R);
    bool caps = ks & KBD_CAPS_LOCK;
    bool upper = shift ^ caps;
    if(ev.code == KEY_A && upper) print('A');


Implemented functions:
1. void kbd_handler_init(void);
    - Input: None
    - Output: None
    - Used for: Initialize the keyboard handler. Without it there wont be any communication between the keyboard and the software. Only use once
    - Example: Inside kernel.c 
        ```c
        // Init keyboard handler
        kbd_handler_init();

2. bool kbd_pop_event(key_event *out);
    - Input: Address of key_event
    - Output: Returns false if
        1. The queue is full
        2. The scancode is 0 (Maps to error scancode)
        3. The scancode-to-key map does not exist (KEY_NONE)
        Otherwise return true
    - Used for: Reading a scancode and translate it to a key event.
    - Example: Reads a valid key event and stores/handles the data 
        ```c
        if(!kbd_pop_event(&event)) return false;
        update_kbd_state(event); // Update keyboard state
        // Store event information
        key->type = INPUT_EV_KEY;
        key->mods = ks;
        key->key.code = event.code;
        key->key.action = event.action;
        key->key.flags = event.flags;

Assumptions made:
1. For the scancode to keycode the only LUTs are for PS/2 Set1 with and without E0 (Located inside drivers/kbd_LUTs/h):
    1. ps2_set1_to_keycode
    2. ps2_set1_e0_to_keycode
2. Keycode to text LUTs are only for US with and without Shift (Located inside drivers/kbd_LUTs/h):
    1. keycode_to_text_us
    2. keycode_to_text_us_shift
3. Currently the keyboard status port 0x64 is not used. Potentially bug sensitive

# VGA Driver

Communicates with VGA display
VGA color text port: 0xB8000
VGA cursor port: 0x3D4

Implemented functions:
1. void vga_clear(void);
    - Input: None
    - Output: None
    - Used for: Clear the VGA (Sets every pixel to whitespace with te same color attributes as the background)

2. void vga_refresh(const uint16_t *fullscreen, size_t size);
    - Input: An array of the screen to be displayed and its size
    - Output: None
    - Used for: Fullscreen display usually after scrolling. Avoid using it for signle char or row changes

3. void vga_set_row(const uint16_t *row, uint8_t idx);
    - Input: A signle row array with size SCREEN_WIDTH and the row index [0...SCREEN_HEIGHT-1]
    - Output: None
    - Used for: Updating a single row on the display

4. void vga_putchar(uint8_t x, uint8_t y, uint16_t c);
    - Input: X and Y cordinates on the screen with (0,0) the top left corner and the character (Must include attributes)
    - Output: None
    - Used for: Setting a signle character on the screen

5. void vga_enable_cursor(bool flag);
    - Input: True/False flag
    - Output: None
    - Used for: Enable/Disable the VGA build-in cursor

6. void vga_set_cursor(uint8_t x, uint8_t y);
    - Input: X and Y cordinates with (0,0) the top left corner
    - Output: None. Returns void if X or Y are outside of screen boundaries.
    - Used for: Setting the cursor position to the desired location on screen.


Useful links for more information:

https://wiki.osdev.org/PS/2_Keyboard
http://wiki.osdev.org/VGA_Hardware
https://en.wikipedia.org/wiki/Video_Graphics_Array