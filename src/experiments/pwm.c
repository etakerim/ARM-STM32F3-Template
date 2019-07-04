#include <stm32f3xx.h>

// PORTA 11 - TIM4_CH1
#define PWM     11

#define PWM_AF  10

int main(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    GPIOA->MODER |= (2 << (PWM << 1));
    GPIOA->AFR[1] |= PWM_AF << 12;

    TIM4->PSC = 200;                                 // Delička f_clk na 40 000 Hz
    TIM4->ARR = SystemCoreClock / TIM4->PSC / 440;   // f = 440
    TIM4->CCR1 = TIM4->ARR / 2;                      // Strieda je 50 %

    TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM4->CCMR1 |= 6 << TIM_CCMR1_OC1M_Pos;          // PWM mód 1

    TIM4->CCMR1 |= TIM_CCMR1_OC1PE;                  // Enable preload register
    TIM4->CCER |= TIM_CCER_CC1E;                     // Pripoj pin na signál
    TIM4->BDTR |= TIM_BDTR_MOE;
    TIM4->CR1 |= TIM_CR1_CEN;                        // Začni!
    TIM4->EGR |= TIM_EGR_UG;

    while (1) {
    }
}
