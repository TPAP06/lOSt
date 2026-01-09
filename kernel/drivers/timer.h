// kernel/drivers/timer.h - PIT timer driver

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Timer configuration
#define TIMER_FREQUENCY 1000  // 1000 Hz = 1ms per tick

// Initialize the PIT timer
void timer_init(void);

// Get system uptime in milliseconds
uint64_t timer_get_uptime_ms(void);

// Get system uptime in seconds
uint64_t timer_get_uptime_seconds(void);

// Sleep for specified milliseconds (blocking)
void timer_sleep_ms(uint32_t ms);

// Sleep for specified seconds (blocking)
void timer_sleep(uint32_t seconds);

// Get ticks since boot
uint64_t timer_get_ticks(void);

// Format uptime as string (output: "1h 23m 45s")
void timer_format_uptime(char *buffer, int max_length);

#endif // TIMER_H