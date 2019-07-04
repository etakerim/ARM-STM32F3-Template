#include "stm32f3xx.h"

#define LED         10
#define INTERVAL    100
#define TIMER_FREQUENCY_HZ      1000
volatile uint32_t timer_counter;

void timer_start(void)
{
	SysTick_Config(SystemCoreClock / TIMER_FREQUENCY_HZ);
}

void timer_sleep(uint32_t ticks)
{
	 timer_counter = ticks;
	 while (timer_counter != 0)
		 ;
}

void timer_tick(void)
{
	if (timer_counter != 0)
		--timer_counter;
}

void SysTick_Handler(void)
{
    timer_tick();
}


int main(void)
{
	timer_start();
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    for (int i = 8; i <= 15; i++) {
        GPIOE->MODER |= (1 << (i << 1));
    }

    while (1) {
        for (int i = 8; i <= 15; i++) {
            GPIOE->ODR ^= (1 << i);
            timer_sleep(INTERVAL);
        }
    }
}
