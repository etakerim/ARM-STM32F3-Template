#include <stm32f3xx.h>
#include <charlcd.h>
#include <gpio.h>
#include <systick.h>

int main(void)
{
    CharLCD disp;
    uint8_t font[] = {4, 4, 14, 14, 31, 31, 4, 4};

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

    lcd_init(&disp, I2C1, 0x3f);
    lcd_clear(&disp);
    lcd_cursor(&disp, true);
    /*lcd_loadcustomchar(&disp, 1, font);
    lcd_cursorpos(&disp, 0, 0);

    //lcd_cursor(&disp, 1);
    lcd_putchar(&disp, 1);
    lcd_print(&disp, 0, 1, "Lesy Slovenska");
    lcd_putchar(&disp, 1);*/
    lcd_puts(&disp, "Miroslav Hajek - ABECEDA zjedla DANKA---");

    while(1) {
        lcd_scroll_left(&disp); 
        systick_sleep(500);
        
    }


}
