#include <stm32f3xx.h>
#include <gpio.h>


struct gpio leds[] = {{GPIOE, 9}, {GPIOE, 11}, {GPIOE, 13}, {GPIOE, 15}};
struct gpio button = {GPIOC, 11};
volatile bool state;

void EXTI15_10_IRQHandler(void)
{
    if (!exti_it_occured(GPIO_EXTI_LINE(button))) {
        for (int i = 0; i < 4; i++) {
            if (state)
                gpio_write(&leds[i], 0);
            else
                gpio_write(&leds[i], 1);
        }
        state = !state;
    }
    exti_it_clearbit(GPIO_EXTI_LINE(button));
}

int main(void)
{
    // LEDs
    rcc_ahb_enable(RCC_AHB(GPIOE), true);

    for (int i = 0; i < 4; i++) {
        gpio_mode(&leds[i], GPIO_OUT);
        gpio_ospeed(&leds[i], GPIO_HIGH_SPEED);
    }

    // Button
    rcc_ahb_enable(RCC_AHB(GPIOC), true);
    gpio_mode(&button, GPIO_IN);
    gpio_pull(&button, GPIO_PULL_DOWN);

    // Setup interrupt
    gpio_extiline_select(&button);
    exti_interrupt_enable(GPIO_EXTI_LINE(button), true);
    exti_trigger_rising(GPIO_EXTI_LINE(button), true);
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    while(1) {

    }
}
