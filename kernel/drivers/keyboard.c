// kernel/drivers/keyboard.c

#include "keyboard.h"
#include "screen.h"
#include "../interrupts/isr.h"
#include "../lib/io.h"
#include "../lib/string.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Circular buffer - use unsigned char for special keys
static unsigned char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_start = 0;
static volatile int buffer_end = 0;

// Keyboard state
static bool shift_pressed = false;
static bool caps_lock = false;
bool ctrl_pressed = false;  // NOT static - so other files can access it
static bool alt_pressed = false;

// Extended scancode tracking
static bool extended_scancode = false;

// US QWERTY scancode to ASCII
static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Add to buffer
static void buffer_add(unsigned char c)
{
    int next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next;
    }
}

// Get from buffer
static unsigned char buffer_get(void)
{
    if (buffer_start == buffer_end) {
        return 0;
    }
    unsigned char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Keyboard interrupt handler
static void keyboard_handler(registers_t *regs)
{
    (void)regs;
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Check for extended scancode prefix (0xE0)
    if (scancode == 0xE0) {
        extended_scancode = true;
        return;
    }

    // Handle key release
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        if (extended_scancode) {
            extended_scancode = false;
        } else {
            // Regular key release
            if (scancode == 0x2A || scancode == 0x36) {
                shift_pressed = false;
            } else if (scancode == 0x1D) {
                ctrl_pressed = false;
            } else if (scancode == 0x38) {
                alt_pressed = false;
            }
        }
        return;
    }
    
    // Handle extended scancodes (arrow keys, etc.)
    if (extended_scancode) {
        extended_scancode = false;
        
        switch (scancode) {
            case 0x48: buffer_add(KEY_UP_ARROW); break;
            case 0x50: buffer_add(KEY_DOWN_ARROW); break;
            case 0x4B: buffer_add(KEY_LEFT_ARROW); break;
            case 0x4D: buffer_add(KEY_RIGHT_ARROW); break;
            case 0x47: buffer_add(KEY_HOME); break;
            case 0x4F: buffer_add(KEY_END); break;
            case 0x53: buffer_add(KEY_DELETE); break;
            case 0x49: buffer_add(KEY_PAGE_UP); break;
            case 0x51: buffer_add(KEY_PAGE_DOWN); break;
        }
        return;
    }
    
    // Handle special keys
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    
    if (scancode == 0x1D) {
        ctrl_pressed = true;
        return;
    }
    
    if (scancode == 0x38) {
        alt_pressed = true;
        return;
    }
    
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return;
    }
    
    // Convert to ASCII
    char c = 0;
    
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            c = scancode_to_ascii_shift[scancode];
        } else {
            c = scancode_to_ascii[scancode];
        }
        
        // Apply caps lock
        if (caps_lock) {
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            } else if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
        }
    }
    
    if (c != 0) {
        buffer_add((unsigned char)c);
    }
}

// Initialize keyboard
void keyboard_init(void)
{
    buffer_start = 0;
    buffer_end = 0;
    shift_pressed = false;
    caps_lock = false;
    ctrl_pressed = false;
    alt_pressed = false;
    extended_scancode = false;
    
    irq_install_handler(1, keyboard_handler);
}

// Check if key available
bool keyboard_available(void)
{
    return buffer_start != buffer_end;
}

// Get character (blocking)
unsigned char keyboard_getchar(void)
{
    while (!keyboard_available()) {
        __asm__ volatile ("hlt");
    }
    return buffer_get();
}

// Read line with history support
void keyboard_readline_history(char *buffer, int max_length,
                                char history[][256], int history_max, 
                                int history_count, int *history_write_pos)
{
    int pos = 0;
    buffer[0] = '\0';
    
    int history_offset = 0;
    char temp_input[256] = {0};
    
    int start_x, start_y;
    screen_get_cursor(&start_x, &start_y);
    
    while (pos < max_length - 1) {
        screen_invert_color();
        unsigned char c = keyboard_getchar();
        
        screen_get_cursor(&start_x, &start_y);
        if (c == '\n') {
            screen_invert_color();
            screen_putchar('\n');
            break;
        }
        else if (c == '\b') {
            if(ctrl_pressed){
                screen_clear_last_word();
            }
            else if (pos > 0) {
                pos--;
                buffer[pos] = '\0';
                screen_putchar('\b');
            }
        }
        else if (c == KEY_UP_ARROW) {
            // Use ctrl_pressed directly (it's in this file)
            if (ctrl_pressed) {
                // Ctrl+Up = Scroll up
                screen_scroll_up();
            } else {
                // Regular Up = Command history
                if (history_count == 0) {
                    continue;
                }
                
                if (history_offset == 0) {
                    strcpy(temp_input, buffer);
                }
                
                if (history_offset > -(history_count)) {
                    history_offset--;
                    
                    int hist_idx = (*history_write_pos + history_offset + history_max) % history_max;
                    
                    screen_set_cursor(start_x, start_y);
                    for (int i = 0; i < pos; i++) {
                        screen_putchar(' ');
                    }
                    screen_set_cursor(start_x, start_y);
                    
                    strcpy(buffer, history[hist_idx]);
                    pos = strlen(buffer);
                    screen_write(buffer);
                }
            }
        }
        else if (c == KEY_DOWN_ARROW) {
            // Use ctrl_pressed directly
            if (ctrl_pressed) {
                // Ctrl+Down = Scroll down
                screen_scroll_down();
            } else {
                // Regular Down = Command history
                if (history_offset < 0) {
                    history_offset++;
                    
                    screen_set_cursor(start_x, start_y);
                    for (int i = 0; i < pos; i++) {
                        screen_putchar(' ');
                    }
                    screen_set_cursor(start_x, start_y);
                    
                    if (history_offset == 0) {
                        strcpy(buffer, temp_input);
                    } else {
                        int hist_idx = (*history_write_pos + history_offset + history_max) % history_max;
                        strcpy(buffer, history[hist_idx]);
                    }
                    
                    pos = strlen(buffer);
                    screen_write(buffer);
                }
            }
        }
        else if(c == KEY_LEFT_ARROW){
            //  if use ctrl just to start of line (0 is reserved so the line starts at x = 1).
            screen_invert_color();

            if(ctrl_pressed){
                pos = 0;
                screen_set_cursor(2, start_y);
            } else if(pos > 0){
                pos--;
                // Regular Left = move by one to the left
                screen_set_cursor(start_x-1, start_y);
            }
        }
        else if(c == KEY_RIGHT_ARROW){
            screen_invert_color();
            
            uint8_t line_len = screen_get_line_len();
            
            if(ctrl_pressed){
                pos = line_len;
                // Jump to the end of line
                screen_set_cursor(line_len, start_y);
            } else if(start_x < line_len){  // Hardcoded to not allow the cursor to move on the void
                pos++;
                // Regular Left = move by one to the left
                screen_set_cursor(start_x+1, start_y);
            }
        }
        else if (c >= 32 && c < 127) {
            if (history_offset != 0) {
                history_offset = 0;
            }
            
            buffer[pos] = (char)c;
            pos++;
            buffer[pos] = '\0';
            screen_putchar((char)c);
        }
    }
    
    buffer[pos] = '\0';
}