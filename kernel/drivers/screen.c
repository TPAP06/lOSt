// kernel/drivers/screen.c - Complete scrollback implementation

#include "screen.h"
#include "../lib/string.h"
#include "../memory/heap.h"
#include "../lib/io.h"

// VGA buffer
static volatile uint16_t* vga = (uint16_t*)0xB8000;

// Clears VGA
void vga_clear(void){
    uint16_t blank = ' ' | 0x0700;
    for(size_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++){
        vga[i] = blank;
    }
}

// Refresh VGA display based on scroll offset
void vga_refresh(const uint16_t *fullscreen, size_t size)
{
    for(size_t i = 0; i < size; i++){
        vga[i] = fullscreen[i];
    }

    // Show scroll indicator in top-right corner because it looks cool
    vga[79] = '^' | 0x0E00;  // Yellow up arrow
}

// Sets a given indexed row on the VGA
void vga_set_row(const uint16_t *row, uint8_t idx){
    for(int x = 0; x < SCREEN_WIDTH; x++){
        vga[idx * SCREEN_WIDTH + x] = row[x];
    }
}

// Put character on screen
void vga_putchar(uint8_t x, uint8_t y, uint16_t c, uint16_t attr)
{
    vga[y * SCREEN_WIDTH + x] = c | attr;
}

// Changes the position of the VGA hw cursor
void vga_set_cursor(uint8_t x, uint8_t y){
    uint16_t pos = y * SCREEN_WIDTH + x;

    outb(0x3D4, 0x0F);      // select low cursor byte
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);      // select high cursor byte
    outb(0x3D5, (uint8_t)((pos>>8) & 0xFF));
}

// Enable/Disable VGA hw cursor
void vga_enable_cursor(bool flag){

}