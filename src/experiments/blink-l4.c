#include <stm32l4xx.h>
#include <cmsis_gcc.h>

#define LED 		3

int main()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    GPIOB->MODER &= ~(3 << (LED * 2));
    GPIOB->MODER |= (1 << (LED * 2));
    GPIOB->MODER &= ~(1 << LED);

    while(1)
    {
        GPIOB->ODR ^= (1 << LED);
        for (volatile uint32_t i = 0; i < 500000; i++)
            __NOP();
    }
}
