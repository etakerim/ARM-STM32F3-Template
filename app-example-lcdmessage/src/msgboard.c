#include <stm32f3xx.h>
#include <gpio.h>
#include <usart.h>
#include <charlcd.h>
#include <systick.h>
#include <string.h>
#include "ltm_parser.h"


void bluetooth_setup(void)
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
}

void lcd_setup(void)
{
    rcc_ahb_enable(RCC_AHBENR_GPIOBEN, true);
    rcc_apb1_enable(RCC_APB1ENR_I2C1EN, true);

    struct gpio scl = {GPIOB, 8};
    struct gpio sda = {GPIOB, 9};

    gpio_mode(&scl, GPIO_AF);
    gpio_otype(&scl, GPIO_OPENDRAIN);
    gpio_ospeed(&scl, GPIO_HIGH_SPEED);
    gpio_alternate(&scl, 4);

    gpio_mode(&sda, GPIO_AF);
    gpio_otype(&sda, GPIO_OPENDRAIN);
    gpio_ospeed(&sda, GPIO_HIGH_SPEED);
    gpio_alternate(&sda, 4);

    NVIC_EnableIRQ(I2C1_EV_IRQn);
}

struct gpio button = {GPIOC, 11};
volatile bool btn_event = false;

void EXTI15_10_IRQHandler(void)
{
    if (!exti_it_occured(button.pin) && !btn_event) {
        btn_event = true;
    }
    exti_it_clearbit(button.pin);
}

void button_setup(void)
{
    rcc_ahb_enable(RCC_AHBENR_GPIOCEN, true);
    gpio_mode(&button, GPIO_IN);
    gpio_pull(&button, GPIO_PULL_DOWN);

    gpio_extiline_select(&button);
    exti_interrupt_enable(button.pin, true);
    exti_trigger_rising(button.pin, true);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}


#define BUFFER_SIZE     512
#define LCD_VIEWLINE    16
#define LCD_RAMLINE     40
#define SCROLL_CHARTIME 400

struct viewport {
    volatile bool update;
    char text[BUFFER_SIZE];
    char *curr;  char *send;
    unsigned int isymbol;
    unsigned int linelen;
    unsigned int xcursor;
    char *line2;
};

char *skip_to_newline(char *pos, unsigned int *syms)
{
    uint8_t c;
    unsigned int i;

    for (i = 0; *pos != '\0'; i++) {
        pos = ltm_decode(pos, &c);
        if (c == '\n') {
            break;
        }
    }
    if (syms)
        *syms = i;
    return pos;
}

void view_transfer(CharLCD *lcd, struct viewport *view, uint8_t c)
{
    lcd_cursorpos(lcd, 0, view->xcursor);
    lcd_putchar(lcd, c);
    view->xcursor = (view->xcursor + 1) % LCD_RAMLINE;
}

void view_nextline(CharLCD *lcd, struct viewport *view)
{
    uint8_t c;
    unsigned int nl_len;

    // Return to beginning of string - to repeat scrolling
    if (*view->curr == '\0')
        view->curr = view->text;

    view->line2 = skip_to_newline(view->curr, &view->linelen);

    // Determine if newline will - scroll (in/out) / appear
    view->isymbol = 0;
    if (view->linelen <= LCD_VIEWLINE)
        view->xcursor = 0;
    else
        view->xcursor = LCD_VIEWLINE;

    view->send = view->curr;
    lcd_clear(lcd);

    // Send current line to LCD DDRAM (until '\0', '\n', <RAMLINE)
    while (*view->send != '\0' && view->xcursor < (LCD_RAMLINE - 1)) {
        view->send = ltm_decode(view->send, &c);
        if (c == '\n')
            break;
        view_transfer(lcd, view, c);
    }

    // If no scrolling is required, show both lines
    skip_to_newline(view->line2, &nl_len);
    if (view->linelen <= LCD_VIEWLINE && nl_len <= LCD_VIEWLINE) {
        lcd_cursorpos(lcd, 1, 0);
        char *p = view->line2;

        for (unsigned int i = 0; p != '\0' && i < nl_len; i++) {
            p = ltm_decode(p, &c);
            if (c == '\n')
               break;
            lcd_putchar(lcd, c);
        }
    }
}

void view_scroll(CharLCD *lcd, struct viewport *view)
{
    uint8_t c;
    char *p;

    if (*view->text == '\0')
        return;

    if (view->linelen > LCD_VIEWLINE) {
        lcd_scroll_left(lcd);

        // Manually push charcters on bus where display hardware scroll buffer
        // is not sufficiently large. - Rewrite last shifted out character
        if (view->linelen >= LCD_RAMLINE) {
            p = ltm_decode(view->send, &c);
            if (c != '\0' && c != '\n') {
                view->send = p;
                view_transfer(lcd, view, c);
            }
        }
    }

    // Move one character forward virtually
    p = ltm_decode(view->curr, &c);

    // Checking line / string end and scroll out / display next line
    if (c != '\n' && c != '\0') {
        view->curr = p;
    } else if (view->linelen > LCD_VIEWLINE &&
                   view->isymbol - LCD_VIEWLINE < view->linelen) {
        view_transfer(lcd, view, ' ');
    } else {
        view->curr = p;
        view_nextline(lcd, view);
    }
    view->isymbol++;
}

char bluein[BUFFER_SIZE];
struct viewport viewport;

int main(void)
{
    CharLCD lcd;
    struct ltm cmd;

    lcd_setup();
    lcd_init(&lcd, I2C1, 0x3f);
    bluetooth_setup();
    button_setup();
    systick_start();

    while (1) {
        if (systick_event(SCROLL_CHARTIME)) {
            if (btn_event) {
                btn_event = false;
                viewport.curr = skip_to_newline(viewport.curr, NULL);
                view_nextline(&lcd, &viewport);
            } else {
                view_scroll(&lcd, &viewport);
            }
        }

        if (usart_gets_nb(bluein, BUFFER_SIZE, true)) {
            // usart_print(bluein);
            int status = ltm_parse(&cmd, bluein);

            switch (status) {
            case LTM_OK:
                usart_print(LTM_RESPONSE_OK);

                for (uint8_t i = 0; i < GLYPH_MEMLOC; i++) {
                    if (cmd.active_glyphs[i])
                        lcd_loadcustomchar(&lcd, i, cmd.glyphs[i]);
                }

                memset(&viewport, 0, sizeof(viewport));
                strncpy(viewport.text, cmd.text, BUFFER_SIZE);
                view_nextline(&lcd, &viewport);
                break;

            case LTM_BAD:
                usart_print(LTM_RESPONSE_BAD);
                break;

            case LTM_ECHO:
                usart_print(LTM_RESPONSE_MSG);
                usart_print("\n");
                break;
            }
        }
    }
}
