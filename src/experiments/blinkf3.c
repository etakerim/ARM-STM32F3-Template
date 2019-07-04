#include <stm32f3xx.h>

#define LED 		15

int main()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

    GPIOE->MODER |= (1 << (LED * 2));
    // GPIOE->OTYPER |= (1 << LED);
    GPIOE->ODR |= (1 << LED);

    while(1)
    {
    }
}
