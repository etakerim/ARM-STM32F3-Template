#include "usart.h"


struct queue tx_usart;
struct queue rx_usart;
static bool tx_lock, rx_overflow;


void USART2_IRQHandler(void)
{
    uint8_t c;

    if (USART2->ISR & USART_ISR_RXNE) {
        if (!queue_write(&rx_usart, USART2->RDR))
            rx_overflow = true;
    }

    if (USART2->ISR & USART_ISR_TXE) {
        if (queue_read(&tx_usart, &c)) {
            USART2->TDR = c;
        } else {
            clear_bits(USART2->CR1, USART_CR1_TXEIE);
            tx_lock = false;
        }
    }
}

void usart_putc(char c)
{
    while (!queue_write(&tx_usart, (uint8_t)c))
        ;
    if (!tx_lock) {
        tx_lock = true;
        set_bits(USART2->CR1, USART_CR1_TXEIE);
    }
}

char usart_getc(void)
{
    uint8_t c;

    while (!queue_read(&rx_usart, &c))
        ;
    return c;
}

int usart_gets(char *buffer, int len, bool term)
{
    int i;

    for (i = 0; i < len; i++) {
        buffer[i] = usart_getc();
        if (term && (buffer[i] == '\n' || buffer[i] == '\r')) {
            buffer[i] = '\0';
            break;
        }
    }
    return i;
}

int usart_gets_nb(char *buffer, int len, bool term)
{
    static int i = 0;
    int n = 0;
    char c;

    if (queue_read(&rx_usart, (uint8_t *)&c)) {
        if (i == len || (term && (c == '\n' || c == '\r'))) {
            buffer[i] = '\0';
            n = i;
            i = 0;
        } else {
            buffer[i++] = c;
        }
    }
    return n;
}

void usart_print(const char *str)
{
    while (*str != '\0')
        usart_putc(*str++);
}


void usart_baudrate(USART_TypeDef *usart, uint32_t baud, bool oversample)
{
    uint16_t bd = SystemCoreClock / baud;   // todo: read clock config

    clear_bits(usart->CR1, USART_CR1_OVER8);
    if (oversample) {
        set_bits(usart->CR1, USART_CR1_OVER8);
        bd = (bd & 0xfff0) | ((bd & 0xf) >> 1);
    }
    set_bits(usart->BRR, bd);
}

void usart_databits(USART_TypeDef *usart, enum databits d)
{
    clear_bits(usart->CR1, USART_CR1_M);
    set_bits(usart->CR1, d << USART_CR1_M_Pos);
}

void usart_stopbits(USART_TypeDef *usart, enum stopbits s)
{
    clear_bits(usart->CR1, USART_CR2_STOP);
    set_bits(usart->CR1, s << USART_CR2_STOP_Pos);
}

void usart_parity(USART_TypeDef *usart, enum parity p)
{
    clear_bits(usart->CR1, USART_CR1_PS | USART_CR1_PCE);
    if (p != USART_PARITY_NONE) {
        set_bits(usart->CR1, USART_CR1_PCE);
        set_bits(usart->CR1, p << USART_CR1_PS_Pos);
    }
}

void usart_enable(USART_TypeDef *usart, bool enable)
{
    if (enable) {
        set_bits(usart->CR1, USART_CR1_RE | USART_CR1_TE);
        set_bits(usart->CR1, USART_CR1_RXNEIE);
        set_bits(usart->CR1, USART_CR1_UE);
    } else {
        clear_bits(usart->CR1, USART_CR1_UE);
        clear_bits(usart->CR1, USART_CR1_RXNEIE | USART_CR1_TXEIE);
        clear_bits(usart->CR1, USART_CR1_RE | USART_CR1_TE);
    }
    NVIC_EnableIRQ(USART2_IRQn);
}
