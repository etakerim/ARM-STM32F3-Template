CMSIS_SRC = CMSIS/src/stm32f303
CMSIS_INC = CMSIS/include/stm32f3xx

MCU = cortex-m4
FLASHADDR = 0x8000000
DEFINES =-DSTM32F303xC
# OPENOCD_CONF = board/stm32f3discovery.cfg

PROJECT = msgboard
#SOURCES = src/examples/msgboard.c src/examples/ltm_parser.c
SOURCES = src/experiments/pwm.c

#SOURCES += src/libstm/gpio.c src/libstm/queue.c src/libstm/i2c.c \
#           src/libstm/systick.c src/libstm/usart.c src/libstm/charlcd.c

SOURCES += $(CMSIS_SRC)/system.c  $(CMSIS_SRC)/startup.c\
           $(CMSIS_SRC)/vectors.c
INCLUDES = -ICMSIS/include/generic/ -I$(CMSIS_INC)/ -Isrc/libstm/
LDFLAGS = -T$(CMSIS_SRC)/memory.ld
BUILD_DIR = build
OPTIMIZE = s
#s ,0, 1, 2


COMPILER_PATH = /home/miroslav/Dokumenty/gcc-arm-none-eabi-5_4-2016q3/bin
# OPENOCD_PATH = /home/miroslav/Dokumenty/openocd/0.10.0-7-20180123-1217/bin
STLINK_PATH =

CC       = $(COMPILER_PATH)/arm-none-eabi-gcc
OBJCOPY  = $(COMPILER_PATH)/arm-none-eabi-objcopy
SIZE     = $(COMPILER_PATH)/arm-none-eabi-size
DEBUGGER = $(COMPILER_PATH)/arm-none-eabi-gdb
OBJDUMP  = $(COMPILER_PATH)/arm-none-eabi-objdump
OPENOCD = $(OPENOCD_PATH)/openocd
ST_FLASH  = st-flash
ST_UTIL  = st-util

CFLAGS = -O$(OPTIMIZE) -mcpu=$(MCU) -mthumb -fmessage-length=0 -fsigned-char  \
         -ffunction-sections -fdata-sections -ffreestanding 				  \
         -fno-move-loop-invariants -Wall -Wextra -g3 -std=gnu11               \
         --specs=nosys.specs -nostartfiles -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
         $(INCLUDES) $(DEFINES)
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
ELF = $(BUILD_DIR)/$(PROJECT).elf
BIN = $(BUILD_DIR)/$(PROJECT).bin
HEX = $(BUILD_DIR)/$(PROJECT).hex

all: $(BIN)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

# https://github.com/texane/stlink
flash: $(BIN)
	$(ST_FLASH) write $(BIN) $(FLASHADDR)

debug: $(BIN)
	$(ST_UTIL) &
	$(DEBUGGER) -ex "target extended-remote :4242" -ex "load" $(ELF)
	killall st-util

check: $(BIN)
	$(OBJDUMP) -d $(ELF) | less

size: $(ELF)
	$(SIZE) -B $(ELF)

clean:
	rm -rf $(BIN) $(ELF) $(OBJECTS)

.PHONY: all flash debug check size

# http://openocd.org/
# ocd-flash: $(BIN)
#	$(OPENOCD) -f $(OPENOCD_CONF) -c "init; reset halt; flash write_image erase $(BIN) $(FLASHADDR); reset run; shutdown"
# ocd-debug: $(BIN)
#	$(OPENOCD) -f $(OPENOCD_CONF) &
#	$(DEBUGGER) -ex "target extended-remote :3333" -ex "break main" $(ELF)
#	killall openocd
