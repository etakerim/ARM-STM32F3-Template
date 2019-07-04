#include "systick.h"

static volatile uint32_t timer_counter;
static volatile uint32_t event_counter;

void systick_start(void)
{
    SysTick_Config(SystemCoreClock / TIMER_FREQUENCY_HZ);
}

void systick_sleep(uint32_t ticks)
{
    timer_counter = ticks;
    while (timer_counter != 0)
        ;
}

bool systick_event(uint32_t ticks)
{
    if (event_counter == 0) {
        event_counter = ticks;
        return true;
    }
    return false;
}

void SysTick_Handler(void)
{
    if (timer_counter != 0)
        --timer_counter;
    if (event_counter != 0)
        --event_counter;
}

