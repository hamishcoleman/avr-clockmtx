
all:	clockmtx.hex

MCU:=atmega8

CC:=avr-gcc
CFLAGS:=-Os -std=c99 -mmcu=$(MCU)
OBJCOPY:=avr-objcopy

%.elf: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@
