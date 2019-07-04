#include "i2c.h"


static struct queue tx_i2c;
static bool tx_lock;


void i2c_init(I2C_TypeDef *i2c)
{
    clear_bits(i2c->CR1, I2C_CR1_PE);
    set_bits(i2c->TIMINGR, 0x1 << I2C_TIMINGR_PRESC_Pos);
    set_bits(i2c->TIMINGR, 0x13 << I2C_TIMINGR_SCLL_Pos);
    set_bits(i2c->TIMINGR, 0xf << I2C_TIMINGR_SCLL_Pos);
    set_bits(i2c->TIMINGR, 0x2 << I2C_TIMINGR_SDADEL_Pos);
    set_bits(i2c->TIMINGR, 0x4 << I2C_TIMINGR_SCLDEL_Pos);
    set_bits(i2c->CR1, I2C_CR1_PE);
}

void i2c_write_byte(I2C_TypeDef *i2c, uint8_t address, uint8_t byte)
{
    clear_bits(i2c->CR2, I2C_CR2_ADD10);    // 7-bit addresses
    clear_bits(i2c->CR2, I2C_CR2_RD_WRN);   // Write
    clear_bits(i2c->CR2, I2C_CR2_SADD);     // Set address
    set_bits(i2c->CR2, (address << 1) << I2C_CR2_SADD_Pos);
    set_bits(i2c->CR2, I2C_CR2_AUTOEND);    // Autogenerate STOP after NBYTES

    clear_bits(i2c->CR2, I2C_CR2_NBYTES);
    set_bits(i2c->CR2, 1 << I2C_CR2_NBYTES_Pos);

    /*
    while (!queue_write(&tx_i2c, byte))
        ;
    if (!tx_lock) {
        tx_lock = true;
        set_bits(i2c->CR1, I2C_CR1_TXIE);
    }
    */
    // TODO: interrupts 
    set_bits(i2c->CR2, I2C_CR2_START);
    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = byte;
}


void I2C1_EV_IRQHandler(void)
{
    uint8_t byte;

    if (I2C1->ISR & I2C_ISR_TXIS) {
        if (queue_read(&tx_i2c, &byte)) {
            I2C1->TXDR = byte;
        } else {
            clear_bits(I2C1->CR1, I2C_CR1_TXIE);
            tx_lock = false;
        }
    }
}
