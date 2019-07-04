// Funguje
#include <stm32f3xx.h>

#define ADC_IN      1   // PA1 - ADC1_IN2

int main(void)
{
    volatile uint32_t calfac = 0;
    volatile uint32_t result = 0;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER |= (3 << (ADC_IN << 1));
    GPIOA->OSPEEDR |= (3 << (ADC_IN << 1));

    RCC->AHBENR |= RCC_AHBENR_ADC12EN;
    // Choose ADC clock to be synchronized with AHB (HCLK)
    ADC12_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos);
    // ADC Volatage Regulator
    ADC1->CR &= ~(3 << ADC_CR_ADVREGEN_Pos);
    ADC1->CR |= (1 << ADC_CR_ADVREGEN_Pos);
    for (volatile int i = 0; i < 50; i++); // Wait 10us

    // Kalibrácia
    ADC1->CR &= ~ADC_CR_ADEN;
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);
    calfac = ADC1->CALFACT & 0x7f;

    //Enable ADC
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC12_COMMON->CSR & ADC_CSR_ADRDY_MST));

    //Select channel
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (2 << ADC_SQR1_SQ1_Pos);

    while (1) {
        //Convert
        ADC1->CR |= ADC_CR_ADSTART;
        while(ADC12_COMMON->CSR & ADC_CSR_EOC_MST);
        result = ADC1->DR;
    }
}
