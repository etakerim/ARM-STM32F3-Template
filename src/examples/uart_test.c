#include <stm32f3xx.h>
#include <gpio.h>
#include <usart.h>
//#include <systick.h>

int main(void)
{
    rcc_ahb_enable(RCC_AHBENR_GPIODEN, true);
    rcc_apb1_enable(RCC_APB1ENR_USART2EN, true);

    struct gpio tx = {GPIOD, 5};
    struct gpio rx = {GPIOD, 6};

    gpio_mode(&tx, GPIO_AF);
    gpio_alternate(&tx, 7);
    gpio_mode(&rx, GPIO_AF);
    gpio_alternate(&rx, 7);

    usart_baudrate(USART2, 9600, false);
    usart_databits(USART2, USART_DATA_8);
    usart_stopbits(USART2, USART_STOP_1);
    usart_parity(USART2, USART_PARITY_NONE);
    usart_enable(USART2, true);
    NVIC_EnableIRQ(USART2_IRQn);

    while (1) {
        usart_putc(usart_getc());
    }
}
