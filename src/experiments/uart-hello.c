// Funguje
#include <stm32f3xx.h>

// PORTD
#define UART_TX     5
#define UART_RX     6

void usart_putc(char c) {
    while (!(USART2->ISR & USART_ISR_TXE))
        ;
    USART2->TDR = (c & 0xff);
}

char usart_getc() {
    while (!(USART2->ISR & USART_ISR_RXNE))
        ;
    return USART2->RDR;
}

int main(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // TX
    GPIOD->MODER |= (2 << (UART_TX << 1));
    GPIOD->AFR[0] |= (7 << (UART_TX << 2));

    //RX
    GPIOD->MODER |= (2 << (UART_RX << 1));
    GPIOD->AFR[0] |= (7 << (UART_RX << 2));

    // Pozor na výber správnych hodín - pozri RCC registre
    // Predvolene je vybratí HSI = 8 MHz
    USART2->BRR = SystemCoreClock / 9600;
    USART2->CR1 |= USART_CR1_RE;
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_UE;

    while (1) {
        usart_putc(usart_getc());
    }
}
