#include <stm32f3xx.h>
#include <cmsis_gcc.h>

#define LED_0 		8
#define LED_E 		15
#define BUTTON		11

volatile uint8_t state = 0;

void manage_leds(void)
{
    for (uint8_t i = LED_0; i <= LED_E; i++) {
	    if (state)
            GPIOE->ODR &= ~(1 << i);
        else
            GPIOE->ODR |= (1 << i);
    }
    state = !state;

}

void EXTI15_10_IRQHandler(void)
{
    manage_leds();
    EXTI->PR |= 1 << BUTTON;
}

int main()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    //Tlačidlo PC11
    GPIOC->PUPDR |= (2 << (BUTTON << 1));

    for (uint8_t i = LED_0; i <= LED_E; i++) {
        GPIOE->MODER |= (1 << (i << 1));
    }

    SYSCFG->EXTICR[2] &= ~(0xf << 12);
    SYSCFG->EXTICR[2] |= (2 << 12);
    //SYSCFG->EXTICR[0] = 0x2222;
    //SYSCFG->EXTICR[1] = 0x2222;
    //SYSCFG->EXTICR[2] = 0x2000;
    //SYSCFG->EXTICR[3] = 0x2222;
    EXTI->IMR |= 1 << BUTTON;
    EXTI->RTSR |= 1 << BUTTON;
    // IRQn je podľa SYSCFG registra, kde aktivujeme
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    while(1) {
    }
    /*
     Všeobecne: pin = 7
     index = pin / 4;
     posun = (pin % 4) * 4;
     SYSCFG[index] |= val << posun;
     SYSCFG->EXTICR[BUTTON >> 2] &= ~(0x7 << ((BUTTON % 4) << 2));
     EXTI->IMR |= 1 << BUTTON;
     EXTI->RTSR |= 1 << BUTTON;
     NVIC_EnableIRQ(EXTI0_IRQn);

    while(1) {
        if (GPIOC->IDR & (1 << BUTTON)) {
            manage_leds();
        }
    }*/
}
