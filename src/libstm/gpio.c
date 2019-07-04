#include "gpio.h"


void rcc_ahb_enable(uint32_t dev, bool enable)
{
    if (enable)
        set_bits(RCC->AHBENR, dev);
    else
        clear_bits(RCC->AHBENR, dev);
}

void rcc_apb1_enable(uint32_t dev, bool enable)
{
    if (enable)
        set_bits(RCC->APB1ENR, dev);
    else
        clear_bits(RCC->APB1ENR, dev);
}

void rcc_apb2_enable(uint32_t dev, bool enable)
{
    if (enable)
        set_bits(RCC->APB2ENR, dev);
    else
        clear_bits(RCC->APB2ENR, dev);
}


void gpio_mode(struct gpio *gpio, enum gpio_modes mode)
{
    clear_bits(gpio->port->MODER, 3 << (gpio->pin * 2));
    set_bits(gpio->port->MODER, mode << (gpio->pin * 2));
}

void gpio_otype(struct gpio *gpio, enum gpio_types otype)
{
    clear_bits(gpio->port->OTYPER, 1 << gpio->pin);
    set_bits(gpio->port->OTYPER, otype << gpio->pin);
}

void gpio_ospeed(struct gpio *gpio, enum gpio_speeds ospeed)
{
    clear_bits(gpio->port->OSPEEDR, 3 << (gpio->pin * 2));
    set_bits(gpio->port->OSPEEDR, ospeed << (gpio->pin * 2));
}

void gpio_pull(struct gpio *gpio, enum gpio_pulls pull)
{
    clear_bits(gpio->port->PUPDR, 3 << (gpio->pin * 2));
    set_bits(gpio->port->PUPDR, pull << (gpio->pin * 2));
}

void gpio_alternate(struct gpio *gpio, uint8_t afnumber)
{
    uint8_t index = ((gpio->pin * 4) / 32) & 1;
    uint8_t pos = (gpio->pin * 4) % 32;

    clear_bits(gpio->port->AFR[index], (0xf << pos));
    set_bits(gpio->port->AFR[index], ((afnumber & 0xf) << pos));
}

uint8_t gpio_read(struct gpio *gpio)
{
    return is_bit_set(gpio->port->IDR, gpio->pin);
}

void gpio_write(struct gpio *gpio, uint8_t bit)
{
    if (bit)
        set_bits(gpio->port->BSRR, 1 << gpio->pin);
    else
        set_bits(gpio->port->BRR, 1 << gpio->pin);
}

void gpio_toggle(struct gpio *gpio)
{
    toggle_bits(gpio->port->ODR, 1 << gpio->pin);
}



void gpio_extiline_select(struct gpio *gpio)
{
    uint8_t val = 0;
    if (gpio->port == GPIOA)
        val = 0;
    else if (gpio->port == GPIOB)
        val = 1;
    else if (gpio->port == GPIOC)
        val = 2;
    else if (gpio->port == GPIOD)
        val = 3;
    else if (gpio->port == GPIOE)
        val = 4;
    else if (gpio->port == GPIOF)
        val = 5;

    rcc_apb2_enable(RCC_APB2ENR_SYSCFGEN, true);
    set_bits(SYSCFG->EXTICR[gpio->pin / 4],
             val << ((gpio->pin % 4) * 4));
}

void exti_interrupt_enable(uint8_t line, bool enable)
{
    if (enable)
        set_bits(EXTI->IMR, 1 << line);
    else
        clear_bits(EXTI->IMR, 1 << line);
}

void exti_event_enable(uint8_t line, bool enable)
{
    if (enable)
        set_bits(EXTI->EMR, 1 << line);
    else
        clear_bits(EXTI->EMR, 1 << line);
}

void exti_trigger_rising(uint8_t line, bool enable)
{
    if (enable)
        set_bits(EXTI->RTSR, 1 << line);
    else
        clear_bits(EXTI->RTSR, 1 << line);
}

void exti_trigger_falling(uint8_t line, bool enable)
{
    if (enable)
        set_bits(EXTI->FTSR, 1 << line);
    else
        clear_bits(EXTI->FTSR, 1 << line);
}


void exti_software_call(uint8_t line)
{
    set_bits(EXTI->SWIER, 1 << line);
}

uint8_t exti_it_occured(uint8_t line)
{
    return is_bit_set(EXTI->PR, line);
}

void exti_it_clearbit(uint8_t line)
{
    set_bits(EXTI->PR, (1 << line));
}
