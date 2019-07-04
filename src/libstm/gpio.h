#ifndef GPIO_H
#define GPIO_H

#include <stm32f3xx.h>
#include <stdint.h>
#include <stdbool.h>


#define mask(VAL, POS)               ((VAL) << (POS))
#define is_bit_set(REG, POS)        ((REG) & (1 << (POS)))
#define set_bits(REG, MASK)          ((REG) |= (MASK))
#define clear_bits(REG, MASK)        ((REG) &= ~(MASK))
#define toggle_bits(REG, MASK)       ((REG) ^= (MASK))

#define RCC_AHB(NAME)  (RCC_AHBENR_ ## NAME ## EN)
#define RCC_APB1(NAME)  (RCC_APB1ENR_ ## NAME ## EN)
#define RCC_APB2(NAME)  (RCC_APB2ENR_ ## NAME ## EN)

#define GPIO_EXTI_LINE(GPIO)   ((GPIO).pin)


struct gpio {
    GPIO_TypeDef *port;
    uint8_t pin;
};

enum gpio_modes {
    GPIO_IN = 0,
    GPIO_OUT = 1,
    GPIO_AF = 2,
    GPIO_AIN = 3
};

enum gpio_types {
    GPIO_PULLPUSH = 0,
    GPIO_OPENDRAIN = 1
};

enum gpio_speeds {
    GPIO_LOW_SPEED = 0,
    GPIO_MEDIUM_SPEED = 1,
    GPIO_HIGH_SPEED = 3
};

enum gpio_pulls {
    GPIO_NO_PULL = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
};


void rcc_ahb_enable(uint32_t dev, bool enable);
void rcc_apb1_enable(uint32_t dev, bool enable);
void rcc_apb2_enable(uint32_t dev, bool enable);

void gpio_mode(struct gpio *gpio, enum gpio_modes mode);
void gpio_otype(struct gpio *gpio, enum gpio_types otype);
void gpio_ospeed(struct gpio *gpio, enum gpio_speeds ospeed);
void gpio_pull(struct gpio *gpio, enum gpio_pulls pull);
void gpio_alternate(struct gpio *gpio, uint8_t afnumber);

uint8_t gpio_read(struct gpio *gpio);
void gpio_write(struct gpio *gpio, uint8_t bit);
void gpio_toggle(struct gpio *gpio);

void gpio_extiline_select(struct gpio *gpio);
void exti_interrupt_enable(uint8_t line, bool enable);
void exti_event_enable(uint8_t line, bool enable);
void exti_trigger_rising(uint8_t line, bool enable);
void exti_trigger_falling(uint8_t line, bool enable);
void exti_software_call(uint8_t line);
uint8_t exti_it_occured(uint8_t line);
void exti_it_clearbit(uint8_t line);

#endif
