// Funguje
#include <stm32f3xx.h>

//PA4 - DAC1_OUT1
#define DAC_OUT     4

int main(void)
{
    volatile uint32_t out = 0;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= (3 << (DAC_OUT << 1));

    RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;
    DAC1->CR |= DAC_CR_EN1 | DAC_CR_BOFF1;
    DAC1->DHR12R1 = 2047;
    out = DAC1->DOR1;
    while (1) {
    }
}
