// Funguje
#include <stm32f3xx.h>

// PORTB
#define I2C_SDA     9
#define I2C_SCL     8

void i2c_send(uint8_t data)
{
    I2C1->CR2 &= ~I2C_CR2_ADD10;     // 7-bit adress
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;    // write mode
    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |= (0x3f << 1) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= I2C_CR2_AUTOEND;

    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= 1 << I2C_CR2_NBYTES_Pos;
    I2C1->CR2 |= I2C_CR2_START;
    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = data;
}

void delay(int n)
{
    for (volatile int i = 0; i < n; i++)
        ;
}

int main(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    GPIOB->MODER |= (2 << (I2C_SDA << 1));
    GPIOB->OTYPER |= 1 << I2C_SDA;
    GPIOB->OSPEEDR |= (3 << (I2C_SDA << 1));
    GPIOB->AFR[1] |= (4 << 4);

    GPIOB->MODER |= (2 << (I2C_SCL << 1));
    GPIOB->OTYPER |= 1 << I2C_SCL;
    GPIOB->OSPEEDR |= (3 << (I2C_SCL << 1));
    GPIOB->AFR[1] |= 4;

    I2C1->CR1 &= ~I2C_CR1_PE;

    I2C1->TIMINGR |= 0x1 << I2C_TIMINGR_PRESC_Pos;
    I2C1->TIMINGR |= 0x13 << I2C_TIMINGR_SCLL_Pos;
    I2C1->TIMINGR |= 0xf << I2C_TIMINGR_SCLL_Pos;
    I2C1->TIMINGR |= 0x2 << I2C_TIMINGR_SDADEL_Pos;
    I2C1->TIMINGR |= 0x4 << I2C_TIMINGR_SCLDEL_Pos;

    I2C1->CR1 |= I2C_CR1_PE;
    while (1) {
        i2c_send(0x00);
        delay(1000000);
        i2c_send(0x08);
        delay(1000000);
    }
}


