// For this to be called, the project linker must be configured without
// the startup sequence (-nostartfiles).
#include <stdint.h>
#include <sys/types.h>
#include "stm32l4xx.h"

// Defined in linker script
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;

// These magic symbols are provided by the linker.
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));
extern void (*__fini_array_start[]) (void) __attribute__((weak));
extern void (*__fini_array_end[]) (void) __attribute__((weak));


extern int main(void);
// The implementation for the exit routine; for embedded
// applications, a system reset will be performed.
extern void __attribute__((noreturn)) _exit(int);
extern void __attribute__((noreturn)) NVIC_SystemReset(void);


// This is the default hardware reset routine; it can be
// redefined in the application for more complex applications.
// Called from _exit().
void __attribute__((weak,noreturn)) __reset_hardware()
{
    NVIC_SystemReset();
}


// Iterate over all the preinit/init routines (mainly static constructors).
inline void __attribute__((always_inline)) __run_init_array(void)
{
    int count;
    int i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i]();

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
        __init_array_start[i]();
}

// Run all the cleanup routines (mainly static destructors).
inline void __attribute__((always_inline)) __run_fini_array(void)
{
    int count;
    int i;

    count = __fini_array_end - __fini_array_start;
    for (i = count; i > 0; i--)
        __fini_array_start[i - 1]();
}

// This is the place where Cortex-M core will go immediately after reset,
// via a call or jump from the Reset_Handler.

void __attribute__ ((section(".after_vectors"),noreturn,weak))
_start(void)
{
    unsigned long *src, *dst;
    src = &_sidata;
    dst = &_sdata;

    SystemInit();

    // Copy the DATA segment from Flash to RAM
    while (dst < &_edata)
        *dst++ = *src++;

    // Zero fill the BSS section.
    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    SystemCoreClockUpdate();
    __run_init_array();

    int code = main();

    __run_fini_array();
    _exit(code);
    while (1) {}
}
