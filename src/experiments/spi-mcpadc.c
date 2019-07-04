#include <stm32f3xx.h>

// PORTB - AF5 SPI2
#define CS      12
#define SCK     13
#define MOSI    14
#define MISO    15

int main(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER |= (1 << (CS << 1));
    GPIOB->OSPEEDR |= (3 << (CS << 1));

    GPIOB->MODER |= (2 << (SCK << 1));
    GPIOB->OSPEEDR |= (3 << (SCK << 1));
    GPIOB->AFR[1] |= (5 << 20);

    GPIOB->MODER |= (2 << (MOSI << 1));
    GPIOB->OSPEEDR |= (3 << (MOSI << 1));
    GPIOB->AFR[1] |= (5 << 24);

    GPIOB->PUPDR |= (1 << (MISO << 1));
    GPIOB->OSPEEDR |= (3 << (MISO << 1));

    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    // page 730
    SPI2->CR1 |= (5 << SPI_CR1_BR_Pos);
    SPI2->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
    SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
    SPI2->CR2 |= (0xf << SPI_CR2_DS_Pos);
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
    SPI2->CR1 |= SPI_CR1_SSM;
    SPI2->CR1 |= SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_SPE;

    volatile uint16_t packet = 0;

    while (1) {
        GPIOB->ODR &= ~(1 << CS);
        while (!(SPI2->SR & SPI_SR_TXE))
                ;
        packet = SPI2->DR;
        GPIOB->ODR |= (1 << CS);
    }
}
