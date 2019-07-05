## ARM STM32 F3 Project Template
The starting point for low level driver development and testing on STM32 boards with ARM Cortex MCUs. There are few options to use or base of off such as [STM HAL](https://www.st.com/content/ccc/resource/technical/document/user_manual/2f/71/ba/b8/75/54/47/cf/DM00105879.pdf/files/DM00105879.pdf/jcr:content/translations/en.DM00105879.pdf) or [libopencm3](https://github.com/libopencm3/libopencm) which are well described [here](https://www.root.cz/clanky/pristupy-k-programovani-stm32/). Approach taken by this template is to have as small code size and dependencies as possible, therefore it includes primarily CMSIS library with vendor specific boot scripts.

### Prerequisites

Install the newest cross compiler for ARM `gcc-arm-none-eabi` alongside with *binutils*. Use either [ST-Link](https://github.com/texane/stlink) or [OpenOCD](http://openocd.org/) to flash and debug your application. Then edit **Makefile** according to your needs.

#### Focus on setting these variables:

- `MCU`, `FLASHADDR`, `DEFINES` = Check if they match your target board
- `PROJECT` = Name for executable and bin file

- `SOURCES` = Put your C source files here

- `INCLUDES` = Specify custom location of headers

- `COMPLILER_PATH` = Path to cross compiler toolchain

### Build

- `make all`  -  Compile, link and create executable and binary for target architecture
- `make flash`  -  Send binary to board / dedicated programmer and write in flash
- `make debug` - Launch JTAG debugging with st-util in background and attaches gdb to it.
- `make clean`  -  Remove objects and executable. Useful for complete rebuild.

### CMSIS core file structure

Currently template provides out of box support for stm32f303, stm32l432. Other devices have not been tested, but it is easily extendable with other boards. It requires placing vendor specific files correctly into directory structure and modifying makefile.

```
CMSIS/
    include/
        generic/				# CMSIS-CORE Standard Files (ARM)
            > core_cm4.h		# CPU & Core Access
            > cmsis_gcc.h		# CPU & SIMD Instruction Access + Core peripherals
        stm32f3xx/
            > stm32f3xx.h		# Vendor: Device Peripheral Access
    src/
        stm32f303/
            > startup.c			# Device Startup Interrupt Vectors
            > system.c			# Vendor: System & Clock Configuration
            > vectors.c			# Vendor: Table of Interrupt Vectors
            > memory.ld			# MCU: Memory layout of given MCU
```

### Low level abstraction layer - libstm

#### RCC (Reset and Clock Control) - `gpio.h`

```c
void rcc_ahb_enable(uint32_t dev, bool enable);
void rcc_apb1_enable(uint32_t dev, bool enable);
void rcc_apb2_enable(uint32_t dev, bool enable);
```

Enable or disable clock signal to peripheral on specific bus. Use macros to specify the device: `RCC_AHB(peripheral)`. Unless the clock is multiplied, system is running at crystal speed of 8 MHz. AHB/APB bridge determines maximum configurable clock speed available to the peripheral (72 / 36 MHz).

#### GPIO (General Purpose Input Output) - `gpio.h`

```c
void gpio_mode(struct gpio *gpio, enum gpio_modes mode);
```
Pin modes: input (`GPIO_IN`),  output (`GPIO_OUT`), alternate function (`GPIO_AF`)  or analog IO (`GPIO_AIN`).

```c
void gpio_otype(struct gpio *gpio, enum gpio_types otype);
```
Output states: pull-push(`GPIO_PULLPUSH`), open drain + pull-up/down(`GPIO_OPENDRAIN`)

```c
void gpio_ospeed(struct gpio *gpio, enum gpio_speeds ospeed);
```
Output speeds: 2 MHz Low speed(`GPIO_LOW_SPEED`), 10 MHz Medium speed (`GPIO_MEDIUM_SPEED`), 50 MHz High speed (`GPIO_HIGH_SPEED`)

```c
void gpio_pull(struct gpio *gpio, enum gpio_pulls pull);
```
Pull-up/down: `GPIO_NO_PULL`, `GPIO_PULL_UP `, `GPIO_PULL_DOWN`

```c
void gpio_alternate(struct gpio *gpio, uint8_t afnumber);
```
Alternate pin function - lookup table in the datasheet to determine number to activate peripheral

```c
uint8_t gpio_read(struct gpio *gpio);
```
Read input pin and return boolean of its current state.

```c
void gpio_write(struct gpio *gpio, uint8_t bit);
```
Write boolean to output pin.

```c
void gpio_toggle(struct gpio *gpio);
```
Turn pin output state from true to false and vise versa. Faster then reading state and output its inverse.

**Example**

```c
#include <stm32f3xx.h>
#include <gpio.h>

struct gpio led = {GPIOE, 9}
struct gpio button = {GPIOC, 11}

int main(void)
{
	// LED
    rcc_ahb_enable(RCC_AHB(GPIOE), true);
    gpio_mode(&led, GPIO_OUT);
    gpio_ospeed(&led, GPIO_HIGH_SPEED);

    // Button
    rcc_ahb_enable(RCC_AHB(GPIOC), true);
    gpio_mode(&button, GPIO_IN);
    gpio_pull(&button, GPIO_PULL_DOWN);

    while (true) {
        if (gpio_read(&button))
            gpio_write(&led, 1);
        else
            gpio_write(&led, 0);
    }
}
```

#### EXTI (External Interrupts) - `gpio.h`

#### SysTick (System Timer) - `systick.h`

#### UART (Universal Asynchronous Receiver Transmitter) - `usart.h`

#### I2C (Inter-Integrated Circuit) - `i2c.h`
