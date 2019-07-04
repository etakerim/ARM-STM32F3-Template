#ifndef UART_H
#define UART_H

#include <stm32f3xx.h>
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "queue.h"

enum databits {
    USART_DATA_8 = 0,
    USART_DATA_9 = 1
};

enum stopbits {
    USART_STOP_1 = 0,
    USART_STOP_2 = 2,
    USART_STOP_1_5 = 3
};

enum parity {
    USART_PARITY_NONE = -1,
    USART_PARITY_EVEN = 0,
    USART_PARITY_ODD = 1
};

void usart_baudrate(USART_TypeDef *usart, uint32_t baud, bool oversample);
void usart_databits(USART_TypeDef *usart, enum databits d);
void usart_stopbits(USART_TypeDef *usart, enum stopbits s);
void usart_parity(USART_TypeDef *usart, enum parity p);
void usart_enable(USART_TypeDef *usart, bool enable);

bool usart_kbhit(void);
char usart_getc(void);
void usart_putc(char c);
int usart_gets(char *buffer, int len, bool term);
int usart_gets_nb(char *buffer, int len, bool term);
void usart_print(const char *str);

#endif
