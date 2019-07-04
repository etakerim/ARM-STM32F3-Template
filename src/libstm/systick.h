#ifndef SYSTICK_H
#define SYSTICK_H

#include <stm32f3xx.h>
#include <stdint.h>
#include <stdbool.h>

#define TIMER_FREQUENCY_HZ      1000

typedef void (*event_t)(void);

void systick_start(void);
void systick_sleep(uint32_t ticks);
bool systick_event(uint32_t ticks);

#endif
