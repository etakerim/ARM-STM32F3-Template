#include <stm32f3xx.h>
#include <gpio.h>
#include <i2c.h>
#include <systick.h>


int main(void)
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

    i2c_init(I2C1);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    systimer_start();

    uint8_t x = 0;
    while (1) {
        /*i2c_write_byte(I2C1, 0x20, x);
        systimer_sleep(500);
        x++;
        */
        i2c_write_byte(I2C1, 0x3f, 0x0);
        systimer_sleep(1000);
        i2c_write_byte(I2C1, 0x3f, 0x08);
        systimer_sleep(1000);
    }
}
