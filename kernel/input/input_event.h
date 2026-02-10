// kernel/input/input_event.h

#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include <stdint.h>
#include <stdbool.h>

#include "../drivers/keyboard.h"

typedef enum{
    INPUT_EV_KEY,
    INPUT_EV_TEXT
}input_event_type;

typedef struct
{
    input_event_type type;
    kbd_mods mods;
    
    union
    {
        struct{
            keycode_t code;
            key_action action;
            key_flags flags;
        }key;
        struct{
            uint32_t codepoint; // 32 bit to account for all possible Unicodes
        }text;
    };
}input_event;


#endif // INPUT_EVENT_H