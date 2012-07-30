
all:	clockmtx.hex

MCU:=atmega8

ISP:=USBasp
MCU_AVRDUDE:=m8

CC:=avr-gcc
CFLAGS:=-Os -std=c99 -mmcu=$(MCU)
OBJCOPY:=avr-objcopy

burn:	clockmtx.hex
	avrdude -c $(ISP) -p $(MCU_AVRDUDE) -U flash:w:$<:i

%.elf: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@
