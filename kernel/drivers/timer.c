// kernel/drivers/timer.c - PIT timer implementation

#include "timer.h"
#include "../interrupts/isr.h"
#include "../lib/io.h"
#include "../lib/string.h"

// PIT I/O ports
#define PIT_CHANNEL_0  0x40
#define PIT_CHANNEL_1  0x41
#define PIT_CHANNEL_2  0x42
#define PIT_COMMAND    0x43

// PIT frequency
#define PIT_BASE_FREQUENCY 1193182  // Hz

// Global tick counter
static volatile uint64_t system_ticks = 0;

// Timer interrupt handler
static void timer_handler(registers_t *regs)
{
    (void)regs;
    system_ticks++;
}

// Initialize the PIT
void timer_init(void)
{
    // Calculate divisor for desired frequency
    uint32_t divisor = PIT_BASE_FREQUENCY / TIMER_FREQUENCY;
    
    // Send command byte (channel 0, access mode lobyte/hibyte, mode 2)
    // Mode 2 = rate generator
    // 00 = channel 0
    // 11 = lobyte/hibyte access mode
    // 010 = mode 2 (rate generator)
    // 0 = 16-bit binary
    outb(PIT_COMMAND, 0x36);
    
    // Send frequency divisor
    outb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF));         // Low byte
    outb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF));  // High byte
    
    // Install timer interrupt handler (IRQ0)
    irq_install_handler(0, timer_handler);
    
    system_ticks = 0;
}

// Get total ticks since boot
uint64_t timer_get_ticks(void)
{
    return system_ticks;
}

// Get uptime in milliseconds
uint64_t timer_get_uptime_ms(void)
{
    return system_ticks;  // Since we're running at 1000 Hz, ticks = ms
}

// Get uptime in seconds
uint64_t timer_get_uptime_seconds(void)
{
    return system_ticks / 1000;
}

// Sleep for specified milliseconds
void timer_sleep_ms(uint32_t ms)
{
    uint64_t start = system_ticks;
    uint64_t target = start + ms;
    
    while (system_ticks < target) {
        __asm__ volatile ("hlt");  // Wait for interrupt
    }
}

// Sleep for specified seconds
void timer_sleep(uint32_t seconds)
{
    timer_sleep_ms(seconds * 1000);
}

// Format uptime as human-readable string
void timer_format_uptime(char *buffer, int max_length)
{
    uint64_t total_seconds = timer_get_uptime_seconds();
    
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;
    
    // Build string
    char temp[32];
    int pos = 0;
    
    if (hours > 0) {
        itoa(hours, temp, 10);
        int i = 0;
        while (temp[i] && pos < max_length - 1) {
            buffer[pos++] = temp[i++];
        }
        buffer[pos++] = 'h';
        buffer[pos++] = ' ';
    }
    
    if (minutes > 0 || hours > 0) {
        itoa(minutes, temp, 10);
        int i = 0;
        while (temp[i] && pos < max_length - 1) {
            buffer[pos++] = temp[i++];
        }
        buffer[pos++] = 'm';
        buffer[pos++] = ' ';
    }
    
    itoa(seconds, temp, 10);
    int i = 0;
    while (temp[i] && pos < max_length - 1) {
        buffer[pos++] = temp[i++];
    }
    buffer[pos++] = 's';
    buffer[pos] = '\0';
}