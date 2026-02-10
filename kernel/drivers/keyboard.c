// kernel/drivers/keyboard.c

#include "keyboard.h"
#include "../tty/screen_edit.h"
#include "../interrupts/isr.h"
#include "../lib/io.h"
#include "../lib/string.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard event queue size (2^8 for uint8_t indices)
#define KBD_QUEUE_SIZE 256

// Keyboard queue buffer
static uint8_t kbd_queue[KBD_QUEUE_SIZE];

// Queue indices
static uint8_t queue_start;
static uint8_t queue_end;

// Push to queue
static void queue_push(uint8_t sc)
{
    int next = (queue_end + 1) % KBD_QUEUE_SIZE;
    if (next != queue_start) {
        kbd_queue[queue_end] = sc;
        queue_end = next;
    }
}

// Pop from queue
static bool queue_pop(uint8_t *out)
{
    // If queue empty return nothing
    if(queue_start == queue_end) return false;

    *out = kbd_queue[queue_start];
    queue_start = (queue_start + 1) % KBD_QUEUE_SIZE;
    return true;
}

static void kbd_isr_handler(registers_t *regs)
{
    (void)regs;
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    queue_push(scancode);
}


bool kbd_pop_event(key_event *out){
    uint8_t scancode;
    
    // If queue is full return false (error)
    if(!queue_pop(&scancode))   return false;
    if(!scancode)               return false;   // scancode == 0 -> error code by some controllers

    // Init to empty event
    *out = (key_event){0};

    // If queue is empty return;
    if(!scancode) return false;

    // Check for extended
    if(scancode == 0xE0){
        // Flag the event as extended
        out->flags |= KEY_FLAG_EXTENDED;

        // Get the second byte of the extended sequence
        if(!queue_pop(&scancode))   return false;
        if(!scancode)               return false;
    }

    // Set "pressed"
    out->action = (scancode & 0x80) ? RELEASE : PRESS;

    // Set keycode
    if(out->flags & KEY_FLAG_EXTENDED){
        out->code = ps2_set1_e0_to_keycode[scancode & 0x7F];
    } else{
        out->code = ps2_set1_to_keycode[scancode & 0x7F];
    }

    // If keycode is unmapped, drop it
    if(out->code == KEY_NONE) return false;

    return true;
}

void kbd_handler_init(void){
    // Init only indices, don't have to zero-out the queue since we only access previously added data
    queue_end = 0;
    queue_start = 0;

    irq_install_handler(1, kbd_isr_handler);
}